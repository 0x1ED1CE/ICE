#include "ice.h"

#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <dos.h>
#include <go32.h>
#include <dpmi.h>

#include "lib/stb/stb_vorbis.c"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define LOCK_VARIABLE(x) _go32_dpmi_lock_data((void *)&x,(long)sizeof(x));
#define LOCK_FUNCTION(x) _go32_dpmi_lock_code(x,(long)sizeof(x));

#define BUFFER_SIZE 2048

#define MAX_SAMPLES 32
#define MAX_SOURCES 64
#define MAX_STREAMS 8

#define TIMER_IRQ 0x08
#define TIMER_DIV 27   //1193181Hz / 44100Hz

typedef struct {
	unsigned char *data;
	unsigned int   rate;
	unsigned int   length;
} audio_sample;

typedef struct {
	unsigned int  sample_id;
	unsigned int  position;
	unsigned char state;
	unsigned char volume;
} audio_source;

typedef struct {
	stb_vorbis   *ogg_stream;
	unsigned int  channels;
	unsigned int  rate;
	unsigned int  length;
	unsigned int  position;
	unsigned char state;
	unsigned char volume;
} audio_stream;

static audio_sample *samples = NULL;
static audio_source *sources = NULL;
static audio_stream *streams = NULL;

volatile unsigned char *speaker_buffer;
volatile unsigned int   speaker_read;
         unsigned int   speaker_write;

_go32_dpmi_seginfo old_isr;
_go32_dpmi_seginfo new_isr;

unsigned int dsp_port; //Creative Sound Blaster
unsigned int lpt_port; //Covox Speech Thing

unsigned int dsp_reset(
	unsigned int port
) {
	//Reset the DSP
	outportb(port+0x6,1);
	delay(10);
	outportb(port+0x6,0);
	delay(10);

	//Check if reset was successfull
	if (
		((inportb(port+0xE)&0x80)==0x80) && 
		(inportb(port+0xA)==0xAA)
	) { //DSP was found
		return 1;
	}

	return 0;
}

void speaker_isr() {
	if (dsp_port) {
		outportb(
			dsp_port+0xC,
			0x10
		);
		outportb(
			dsp_port+0xC,
			speaker_buffer[speaker_read]
		);
	} else {
		outportb(
			lpt_port,
			speaker_buffer[speaker_read]
		);
	}

	speaker_read += 1;
	speaker_read %= BUFFER_SIZE;
}

void ice_audio_buffer(ice_real tick) {
	unsigned int mix_a;
	unsigned int mix_b;

	audio_sample *sample;
	audio_source *source;
	audio_stream *stream;

	while (speaker_write!=speaker_read) {
		mix_a = 127;

		for (unsigned int i=0; i<MAX_SOURCES; i++) {
			source = &sources[i];
			sample = &samples[source->sample_id];

			if (
				sample->data!=NULL &&
				source->state>ICE_AUDIO_STATE_PAUSED
			) {
				mix_b = (unsigned int)sample->data[source->position];
				mix_a = (mix_a+mix_b)/2;

				source->position += 1;
				source->position %= sample->length;

				if (
					source->position==0 &&
					source->state==ICE_AUDIO_STATE_PLAYING
				) {
					source->state=ICE_AUDIO_STATE_PAUSED;
				}
			}
		}

		speaker_buffer[speaker_write] = (unsigned char)mix_a;

		speaker_write += 1;
		speaker_write %= BUFFER_SIZE;
	};
}

ice_uint ice_audio_init() {
	//Initialize audio buffer
	speaker_buffer = calloc(1,BUFFER_SIZE);
	speaker_read   = 0;
	speaker_write  = 0;

	if (speaker_buffer==NULL) {
		ice_log((ice_char*)"Failed to allocate audio buffer!");

		return 1;
	}
	
	//Detect Sound Blaster
	unsigned int port_offset;
	unsigned int port;

	for (
		port_offset=1; 
		port_offset<9; 
		port_offset++
	) {
		port = 0x200+(port_offset<<4);

		if (
			port_offset!=7 && 
			dsp_reset(port)
		) {
			dsp_port = port;

			ice_char msg[64];
			sprintf(
				(char *)msg,
				"Detected Sound Blaster: %X",
				port
			);
			ice_log(msg);

			break;
		}
	}

	//Switch to Covox if no DSP was found
	if (port_offset==9) {
		ice_log((ice_char*)"Sound Blaster not installed");
		ice_log((ice_char*)"Defaulting to LPT1");

		dsp_port = 0;
		lpt_port = 0x378;
	}

	//Install interrupt service
	LOCK_FUNCTION(speaker_isr);
	LOCK_VARIABLE(speaker_buffer);
	LOCK_VARIABLE(speaker_read);

	_go32_dpmi_get_protected_mode_interrupt_vector(
		TIMER_IRQ,
		&old_isr
	);

	new_isr.pm_offset   = (int)speaker_isr;
	new_isr.pm_selector = _go32_my_cs();

	_go32_dpmi_chain_protected_mode_interrupt_vector(
		TIMER_IRQ,
		&new_isr
	);

	//Set clock divisor to 44191Hz
	outportb(0x43,0x34);
	outportb(0x40,TIMER_DIV&0xFF);
	outportb(0x40,TIMER_DIV>>8);

	//Allocate slots
	ice_log((ice_char*)"Allocating sound slots...");

	samples = calloc(
		MAX_SAMPLES,
		sizeof(audio_sample)
	);

	sources = calloc(
		MAX_SOURCES,
		sizeof(audio_source)
	);

	streams = calloc(
		MAX_STREAMS,
		sizeof(audio_stream)
	);

	if (
		samples == NULL ||
		sources == NULL ||
		streams == NULL
	) {
		ice_log((ice_char *)"Failed to allocate sound slots!");

		return 1;
	}

	return 0;
}

void ice_audio_deinit() {
	if (speaker_buffer==NULL) {
		return;
	}

	free(speaker_buffer);
	speaker_buffer=NULL;

	outportb(0x43,0x34);
	outportb(0x40,0);
	outportb(0x40,0);

	_go32_dpmi_set_protected_mode_interrupt_vector(
		TIMER_IRQ,
		&old_isr
	);

	ice_audio_sample_flush();
	ice_audio_source_flush();
	ice_audio_stream_flush();

	if (samples!=NULL) {
		free(samples);
		samples=NULL;
	}

	if (sources!=NULL) {
		free(sources);
		sources=NULL;
	}

	if (streams!=NULL) {
		free(streams);
		streams=NULL;
		
		return;
	}
}

void ice_audio_sample_flush() {
	if (samples!=NULL) {
		for (ice_uint i=0; i<MAX_SAMPLES; i++) {
			ice_audio_sample_delete(i);
		}
	}
}

ice_uint ice_audio_sample_load(
	ice_uint file_id
) {
	if (samples==NULL) {
		ice_log((ice_char *)"Audio is not initialized!");

		return 0;
	}

	ice_uint      sample_id = 0;
	audio_sample *sample    = NULL;

	for (ice_uint i=0; i<MAX_SAMPLES; i++) {
		if (samples[i].data==NULL) {
			sample_id = i;
			sample    = &samples[i];

			break;
		}
	}

	if (sample==NULL) {
		ice_log((ice_char *)"Exceeded audio samples limit");

		return 0;
	}

	char filename[32];
	sprintf(
		filename,
		"%u.ogg",
		(unsigned int)file_id
	);

	stb_vorbis *stream = stb_vorbis_open_filename(
		filename,
		NULL,
		NULL
	);

	if (stream==NULL) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to load audio sample: %u",
			file_id
		);
		ice_log(msg);

		return 0;
	}

	stb_vorbis_info info = stb_vorbis_get_info(stream);

	sample->rate   = info.sample_rate;
	sample->length = stb_vorbis_stream_length_in_samples(stream);
	sample->data   = malloc(sample->length);

	if (sample->data==NULL) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to allocate audio sample: %u",
			file_id
		);
		ice_log(msg);

		stb_vorbis_close(stream);

		sample->length = 0;
		sample->rate   = 0;

		return 0;
	}

	short buffer[256];
	int   decoded;
	int   data_index = 0;

	do {
		decoded=stb_vorbis_get_samples_short_interleaved(
			stream,
			info.channels,
			buffer,
			256
		);

		for (
			int i=0;
			i<decoded*info.channels;
			i+=info.channels
		) {
			int pulse=32767;

			//Convert 16 bit stereo down to 8 bit mono
			for (int j=0; j<info.channels; j++) {
				pulse = (pulse+buffer[i+j])/2;
			}

			sample->data[data_index]=(unsigned char)(pulse/127);

			data_index++;
		}
	} while (decoded>0);

	stb_vorbis_close(stream);

	return sample_id;
}

void ice_audio_sample_delete(
	ice_uint sample_id
) {
	if (
		samples==NULL ||
		sample_id>=MAX_SAMPLES ||
		samples[sample_id].data==NULL
	) {
		return;
	}

	audio_sample *sample=&samples[sample_id];

	free(sample->data);

	sample->data   = NULL;
	sample->rate   = 0;
	sample->length = 0;
}

ice_real ice_audio_sample_length_get(
	ice_uint sample_id
) {
	if (
		samples==NULL ||
		sample_id>=MAX_SAMPLES ||
		samples[sample_id].data==NULL
	) {
		return 0;
	}

	audio_sample *sample=&samples[sample_id];

	return (ice_real)sample->length/sample->rate;
}

void ice_audio_source_flush() {
	if (sources!=NULL) {
		for (ice_uint i=0; i<MAX_SOURCES; i++) {
			ice_audio_source_delete(i);
		}
	}
}

ice_uint ice_audio_source_new() {
	if (sources==NULL) {
		return 0;
	}

	for (unsigned int i=0; i<MAX_SOURCES; i++) {
		if (sources[i].state==ICE_AUDIO_STATE_NONE) {
			audio_source *source=&sources[i];

			source->state     = ICE_AUDIO_STATE_PAUSED;
			source->sample_id = 0;
			source->position  = 0;
			source->volume    = 255;

			return (ice_uint)i;
		}
	}

	return 0;
}

void ice_audio_source_delete(
	ice_uint source_id
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return;
	}

	audio_source *source=&sources[source_id];

	source->state     = ICE_AUDIO_STATE_NONE;
	source->sample_id = 0;
	source->position  = 0;
	source->volume    = 255;
}

ice_uint ice_audio_source_sample_get(
	ice_uint source_id
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return 0;
	}

	return (ice_uint)sources[source_id].sample_id;
}

void ice_audio_source_sample_set(
	ice_uint source_id,
	ice_uint sample_id
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sample_id>=MAX_SAMPLES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return;
	}

	audio_source *source=&sources[source_id];

	source->sample_id = sample_id;
	source->position  = 0;
}

ice_real ice_audio_source_position_get(
	ice_uint source_id
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return 0;
	}

	audio_source *source=&sources[source_id];

	ice_uint sample_id   = source->sample_id;
	audio_sample *sample = &samples[sample_id];

	if (
		sample_id>=MAX_SAMPLES ||
		sample->data==NULL
	) {
		return 0;
	}

	return (ice_real)sources[source_id].position/sample->rate;
}

void ice_audio_source_position_set(
	ice_uint source_id,
	ice_real position
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return;
	}

	audio_source *source=&sources[source_id];

	ice_uint sample_id   = source->sample_id;
	audio_sample *sample = &samples[sample_id];

	if (
		sample_id>=MAX_SAMPLES ||
		sample->data==NULL
	) {
		return;
	}

	ice_uint sample_position = (ice_uint)(
		position*(ice_real)sample->rate
	);

	source->position=MAX(
		sample_position,
		sample->length
	);
}

ice_uint ice_audio_source_state_get(
	ice_uint source_id
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return ICE_AUDIO_STATE_PAUSED;
	}

	return (ice_uint)sources[source_id].state;
}

void ice_audio_source_state_set(
	ice_uint source_id,
	ice_uint state
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return;
	}

	audio_source *source=&sources[source_id];

	switch(state) {
		case ICE_AUDIO_STATE_PLAYING:
			source->state=ICE_AUDIO_STATE_PLAYING;
			break;
		case ICE_AUDIO_STATE_LOOPING:
			source->state=ICE_AUDIO_STATE_LOOPING;
			break;
		default:
			source->state=ICE_AUDIO_STATE_PAUSED;
	}
}

ice_real ice_audio_source_volume_get(
	ice_uint source_id
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return 0;
	}

	return (ice_real)sources[source_id].volume/255;
}

void ice_audio_source_volume_set(
	ice_uint source_id,
	ice_real volume
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return;
	}

	volume = MAX(volume,0);
	volume = MIN(volume,1);

	sources[source_id].volume=(ice_char)(volume*255);
}

void ice_audio_stream_flush() {
	if (streams!=NULL) {
		for (ice_uint i=0; i<MAX_STREAMS; i++) {
			ice_audio_stream_delete(i);
		}
	}
}

ice_uint ice_audio_stream_load(
	ice_uint file_id
) {
	if (streams==NULL) {
		ice_log((ice_char *)"Audio is not initialized!");

		return 0;
	}

	ice_uint      stream_id = 0;
	audio_stream *stream    = NULL;

	for (ice_uint i=0; i<MAX_STREAMS; i++) {
		if (streams[i].ogg_stream==NULL) {
			stream_id = i;
			stream    = &streams[i];

			break;
		}
	}

	if (stream==NULL) {
		ice_log((ice_char *)"Exceeded audio streams limit");

		return 0;
	}

	char filename[32];
	sprintf(
		filename,
		"%u.ogg",
		(unsigned int)file_id
	);

	stb_vorbis *ogg_stream = stb_vorbis_open_filename(
		filename,
		NULL,
		NULL
	);

	if (ogg_stream==NULL) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to load audio stream: %u",
			file_id
		);
		ice_log(msg);

		return 0;
	}

	stb_vorbis_info info = stb_vorbis_get_info(ogg_stream);

	stream->ogg_stream = ogg_stream;
	stream->channels    = info.channels;
	stream->rate       = info.sample_rate;
	stream->length     = stb_vorbis_stream_length_in_samples(ogg_stream);
	stream->position   = 0;
	stream->volume     = 0;
	stream->state      = ICE_AUDIO_STATE_PAUSED;

	return stream_id;
}

void ice_audio_stream_delete(
	ice_uint stream_id
) {
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return;
	}

	audio_stream *stream = &streams[stream_id];

	stb_vorbis_close(stream->ogg_stream);

	stream->ogg_stream = NULL;
	stream->channels   = 0;
	stream->rate       = 0;
	stream->length     = 0;
	stream->position   = 0;
	stream->volume     = 0;
	stream->state      = ICE_AUDIO_STATE_NONE;
}

ice_real ice_audio_stream_length_get(
	ice_uint stream_id
) {
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return 0;
	}

	audio_stream *stream = &streams[stream_id];

	return (ice_real)stream->length/stream->rate;
}

ice_real ice_audio_stream_position_get(
	ice_uint stream_id
) {
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return 0;
	}

	audio_stream *stream = &streams[stream_id];

	return 0; //TODO
}

void ice_audio_stream_position_set(
	ice_uint stream_id,
	ice_real position
) {
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return;
	}

	audio_stream *stream = &streams[stream_id];

	//TODO
}

ice_uint ice_audio_stream_state_get(
	ice_uint stream_id
) {
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return ICE_AUDIO_STATE_NONE;
	}

	audio_stream *stream = &streams[stream_id];

	return stream->state;
}

void ice_audio_stream_state_set(
	ice_uint stream_id,
	ice_uint state
) {
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return;
	}

	audio_stream *stream = &streams[stream_id];

	switch(state) {
		case ICE_AUDIO_STATE_PLAYING:
			stream->state=ICE_AUDIO_STATE_PLAYING;
			break;
		case ICE_AUDIO_STATE_LOOPING:
			stream->state=ICE_AUDIO_STATE_LOOPING;
			break;
		default:
			stream->state=ICE_AUDIO_STATE_PAUSED;
	}
}

ice_real ice_audio_stream_volume_get(
	ice_uint stream_id
) {
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return 0;
	}

	audio_stream *stream = &streams[stream_id];

	return (ice_real)stream->volume/255;
}

void ice_audio_stream_volume_set(
	ice_uint stream_id,
	ice_real volume
) {
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return;
	}

	audio_stream *stream = &streams[stream_id];

	volume = MAX(volume,0);
	volume = MIN(volume,1);

	stream->volume=(ice_char)(volume*255);
}
