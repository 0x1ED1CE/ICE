#include "ice.h"

#include <stdio.h>
#include <malloc.h>
#include <dos.h>
#include <go32.h>
#include <dpmi.h>

#include "lib/stb/stb_vorbis.c"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define BUFFER_SIZE 4096 //Increase this if audio stream stutters
#define SAMPLE_RATE 22050

#define MAX_SAMPLES 64
#define MAX_SOURCES 32
#define MAX_STREAMS 8

#define TIMER_INT 0x08
#define TIMER_DIV (1193181/SAMPLE_RATE)

typedef struct {
	unsigned char *data;
	unsigned int   rate;
	unsigned int   length;
} audio_sample;

typedef struct {
	volatile unsigned int  sample_id;
	volatile unsigned int  position;
	volatile unsigned char state;
	volatile unsigned char volume;
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

static volatile unsigned char *stream_buffer;
static volatile unsigned int   stream_read;
static          unsigned int   stream_write;

static _go32_dpmi_seginfo old_isr;
static _go32_dpmi_seginfo new_isr;

static unsigned int dsp_port; //Creative Sound Blaster
static unsigned int lpt_port; //Covox Speech Thing

extern volatile unsigned long clock_tick;

unsigned int dsp_reset() {
	unsigned int port_offset;
	unsigned int port;

	for (
		port_offset=1;
		port_offset<9;
		port_offset++
	) {
		port = 0x200+(port_offset<<4);

		outportb(port+0x6,1);
		delay(10);
		outportb(port+0x6,0);
		delay(10);

		if (
			port_offset!=7 &&
			((inportb(port+0xE)&0x80)==0x80) &&
			(inportb(port+0xA)==0xAA)
		) {
			return port; //DSP was found
		}
	}

	return 0;
}

void speaker_isr() {
	unsigned int mix_a = 1;
	unsigned int mix_b = stream_buffer[stream_read%BUFFER_SIZE];

	stream_buffer[stream_read++%BUFFER_SIZE] = 127;

	audio_sample *sample;
	audio_source *source;

	for (unsigned int i=0; i<MAX_SOURCES; i++) {
		source = &sources[i];
		sample = &samples[source->sample_id];

		if (
			sample->data!=NULL &&
			source->state>ICE_AUDIO_STATE_PAUSED
		) {
			mix_a ++;
			mix_b += (unsigned int)sample->data[source->position];

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

	if (dsp_port) {
		outportb(
			dsp_port+0xC,
			0x10
		);
		outportb(
			dsp_port+0xC,
			mix_b/MAX(mix_a,1)
		);
	} else {
		outportb(
			lpt_port,
			mix_b/MAX(mix_a,1)
		);
	}

	clock_tick++;
}

void ice_audio_update() {
	unsigned int todo=MIN(
		stream_read-stream_write,
		BUFFER_SIZE
	);

	unsigned int write_start=stream_read-todo;

	if (todo==0) {
		return;
	}

	stream_read %= BUFFER_SIZE;
	stream_write = stream_read;

	audio_stream *stream;

	for (unsigned int s=0; s<MAX_STREAMS; s++) {
		stream = &streams[s];

		if (stream->state>ICE_AUDIO_STATE_PAUSED) {
			short decoded[todo*stream->channels];
			unsigned int samples = stb_vorbis_get_samples_short_interleaved(
				stream->ogg_stream,
				stream->channels,
				decoded,
				todo*stream->channels
			);

			stream->position += samples;

			if (samples==0) {
				stream->position = 0;

				if (stream->state==ICE_AUDIO_STATE_PLAYING) {
					stream->state=ICE_AUDIO_STATE_PAUSED;
				} else {
					stb_vorbis_seek_start(stream->ogg_stream);
				}

				break;
			}

			for (unsigned int i=0; i<samples; i++) {
				unsigned int buffer_index = (write_start+i)%BUFFER_SIZE;
				unsigned int mix = 0;

				for (unsigned int c=0; c<stream->channels; c++) {
					mix += decoded[i*stream->channels+c]+32767;
				}

				mix /= stream->channels;
				mix /= 256;
				mix  = (mix+stream_buffer[buffer_index])/2;

				stream_buffer[buffer_index] = mix;
			}
		}
	}
}

ice_uint ice_audio_init() {
	//Allocate stream buffer
	ice_log((ice_char*)"Allocating audio stream buffer");

	stream_read   = 0;
	stream_write  = 0;
	stream_buffer = calloc(
		BUFFER_SIZE,
		sizeof(unsigned char)
	);

	if (stream_buffer==NULL) {
		ice_log((ice_char*)"Failed to allocate stream buffer!");

		return 1;
	}

	//Allocate slots
	ice_log((ice_char*)"Allocating sound slots");

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

	dsp_port = dsp_reset(); //Detect DSP
	lpt_port = 0x378;

	if (dsp_port) {
		outportb(dsp_port+0xC,0xD1); //Turn speaker on

		ice_char msg[64];

		sprintf(
			(char *)msg,
			"Detected Sound Blaster: %X",
			dsp_port
		);

		ice_log(msg);
	} else {
		ice_log((ice_char*)"Sound Blaster not installed");
		ice_log((ice_char*)"Defaulting to LPT1");
	}

	ice_log((ice_char *)"Enabling audio playback");

	_go32_dpmi_lock_code(
		speaker_isr,
		(long)sizeof(speaker_isr)
	);

	_go32_dpmi_get_protected_mode_interrupt_vector(
		TIMER_INT,
		&old_isr
	);

	new_isr.pm_offset   = (int)(speaker_isr);
	new_isr.pm_selector = _go32_my_cs();

	_go32_dpmi_chain_protected_mode_interrupt_vector(
		TIMER_INT,
		&new_isr
	);

	//Set PIT clock divider
	outportb(0x43,0x34);
	outportb(0x40,TIMER_DIV&0xFF);
	outportb(0x40,TIMER_DIV>>8);

	return 0;
}

void ice_audio_deinit() {
	ice_log((ice_char *)"Disabling audio playback");

	outportb(0x43,0x34);
	outportb(0x40,0);
	outportb(0x40,0);

	_go32_dpmi_set_protected_mode_interrupt_vector(
		TIMER_INT,
		&old_isr
	);

	if (dsp_port) {
		outportb(dsp_port+0xC,0xD3);
	}

	if (stream_buffer!=NULL) {
		free((void*)stream_buffer);
		stream_buffer=NULL;
	}

	ice_log((ice_char *)"Flushing audio slots");

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
	}
}

ice_uint ice_audio_sample_load(
	ice_uint file_id
) {
	if (samples==NULL) {
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
		"assets\\sounds\\%u.ogg",
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

	if (info.sample_rate!=SAMPLE_RATE) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to load audio sample: %u\n"
			"Audio must be %u kHz",
			file_id,
			SAMPLE_RATE
		);
		ice_log(msg);

		stb_vorbis_close(stream);

		return 0;
	}

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
			int mix=0;

			//Convert 16 bit stereo down to 8 bit mono
			for (int j=0; j<info.channels; j++) {
				mix += buffer[i+j]+32767;
			}

			mix /= info.channels;

			sample->data[data_index++]=(unsigned char)(mix/256);
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

void ice_audio_sample_flush() {
	if (samples!=NULL) {
		for (ice_uint i=0; i<MAX_SAMPLES; i++) {
			ice_audio_sample_delete(i);
		}
	}
}

ice_real ice_audio_sample_length_get(
	ice_uint sample_id
) {
	if (
		sample_id>=MAX_SAMPLES ||
		samples[sample_id].data==NULL
	) {
		return 0;
	}

	audio_sample *sample=&samples[sample_id];

	return (ice_real)sample->length/sample->rate;
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

void ice_audio_source_flush() {
	if (sources!=NULL) {
		for (ice_uint i=0; i<MAX_SOURCES; i++) {
			ice_audio_source_delete(i);
		}
	}
}

ice_uint ice_audio_source_sample_get(
	ice_uint source_id
) {
	if (
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

ice_real ice_audio_source_seek_get(
	ice_uint source_id
) {
	if (
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return 0;
	}

	audio_source *source = &sources[source_id];
	audio_sample *sample = &samples[source->sample_id];

	if (
		source->sample_id>=MAX_SAMPLES ||
		sample->data==NULL
	) {
		return 0;
	}

	return (ice_real)source->position/sample->rate;
}

void ice_audio_source_seek_set(
	ice_uint source_id,
	ice_real position
) {
	if (
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return;
	}

	audio_source *source = &sources[source_id];
	audio_sample *sample = &samples[source->sample_id];

	if (
		source->sample_id>=MAX_SAMPLES ||
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

ice_real ice_audio_source_volume_get(
	ice_uint source_id
) {
	if (
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
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_AUDIO_STATE_NONE
	) {
		return;
	}

	volume = MAX(volume,0);
	volume = MIN(volume,1);

	sources[source_id].volume=(ice_char)(volume*255);
}

ice_uint ice_audio_source_state_get(
	ice_uint source_id
) {
	if (
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

ice_uint ice_audio_stream_load(
	ice_uint file_id
) {
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
		"assets\\sounds\\%u.ogg",
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

	if (info.sample_rate!=SAMPLE_RATE) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to load audio stream: %u\n"
			"Audio must be %u kHz",
			file_id,
			SAMPLE_RATE
		);
		ice_log(msg);

		stb_vorbis_close(ogg_stream);

		return 0;
	}

	stream->ogg_stream = ogg_stream;
	stream->channels   = info.channels;
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

void ice_audio_stream_flush() {
	if (streams!=NULL) {
		for (ice_uint i=0; i<MAX_STREAMS; i++) {
			ice_audio_stream_delete(i);
		}
	}
}

ice_real ice_audio_stream_length_get(
	ice_uint stream_id
) {
	if (
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return 0;
	}

	audio_stream *stream = &streams[stream_id];

	return (ice_real)stream->length/stream->rate;
}

ice_real ice_audio_stream_seek_get(
	ice_uint stream_id
) {
	if (
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return 0;
	}

	audio_stream *stream = &streams[stream_id];

	return (ice_real)stream->position/stream->rate;
}

void ice_audio_stream_seek_set(
	ice_uint stream_id,
	ice_real position
) {
	if (
		stream_id>=MAX_STREAMS ||
		streams[stream_id].ogg_stream==NULL
	) {
		return;
	}

	audio_stream *stream = &streams[stream_id];

	ice_uint stream_position = (ice_uint)(
		position*(ice_real)stream->rate
	);

	stream->position=MAX(
		stream_position,
		stream->length
	);

	stb_vorbis_seek(
		stream->ogg_stream,
		stream->position
	);
}

ice_real ice_audio_stream_volume_get(
	ice_uint stream_id
) {
	if (
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

ice_uint ice_audio_stream_state_get(
	ice_uint stream_id
) {
	if (
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