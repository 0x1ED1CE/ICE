#include <stdint.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <malloc.h>
#include <math.h>
#include <mem.h>
#include <sys\movedata.h>
#include <sys\nearptr.h>
#include <pc.h>
#include <go32.h>
#include <dpmi.h>

#include "ice.h"

#include "lib/stb/stb_vorbis.c"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define BUFFER_SIZE 2048

#define MAX_SAMPLES 32
#define MAX_SOURCES 64
#define MAX_STREAMS 8

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

audio_sample *samples = NULL;
audio_source *sources = NULL;
audio_stream *audio_streams = NULL;

unsigned int   dsp_port;                      //Sound Blaster port address
unsigned int   dsp_dma               = 0x01;  //DMA channel
unsigned char *dsp_dma_buffer        = NULL;
unsigned int   dsp_dma_buffer_offset = 0;

_go32_dpmi_seginfo dos_dma_buffer;
unsigned int       dos_dma_buffer_offset;

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
	) {
		//DSP was found
		dsp_port=port;
		
		return 1;
	}
	
	return 0;
}

void dsp_write(
	unsigned char value
) {
  //Wait for the DSP to be ready to accept data
  while ((inportb(dsp_port+0xC)&0x80)==0x80);
  
  //Send byte
  outportb(dsp_port+0xC,value);
}

void ice_audio_buffer(ice_real tick) {
	if (samples==NULL || sources==NULL) {
		return;
	}
	
	unsigned int pre_offset;
	unsigned int dma_offset;
	unsigned int dma_address;
	
	unsigned int mix_a;
	unsigned int mix_b;
	unsigned int mix_c;
	
	audio_sample *sample;
	audio_source *source;
	audio_stream *stream;
	
	dma_address = inportb(1+(dsp_dma<<1));
	dma_address = dma_address+(inportb(1+(dsp_dma<<1))<<8);
	dma_address = (BUFFER_SIZE-1)-dma_address;
	pre_offset  = dma_offset=dma_address;
	
	while (dsp_dma_buffer_offset!=dma_offset) {
		dsp_dma_buffer[dsp_dma_buffer_offset]=127;
		
		for (unsigned int i=0; i<MAX_SOURCES; i++) {
			source = &sources[i];
			sample = &samples[source->sample_id];
			
			if (
				sample->data!=NULL &&
				source->state>ICE_AUDIO_STATE_PAUSED 
			) {
				mix_a = (unsigned int)dsp_dma_buffer[dsp_dma_buffer_offset];
				mix_b = (unsigned int)sample->data[source->position];///source->volume;
				mix_c = (mix_a+mix_b)/2;
				
				dsp_dma_buffer[dsp_dma_buffer_offset]=(unsigned char)mix_c;
				
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
		
		dsp_dma_buffer_offset += 1;
		dsp_dma_buffer_offset &= (BUFFER_SIZE-1);
	};
	
	//Copy as one or in parts
	if (dsp_dma_buffer_offset>pre_offset) {
		dosmemput(
			dsp_dma_buffer+pre_offset,
			dsp_dma_buffer_offset-pre_offset,
			dos_dma_buffer_offset+pre_offset
		);
	} else {
		dosmemput(
			dsp_dma_buffer+pre_offset,
			BUFFER_SIZE-pre_offset,
			dos_dma_buffer_offset+pre_offset
		);
		dosmemput(
			dsp_dma_buffer,
			dsp_dma_buffer_offset,
			dos_dma_buffer_offset
		);
	}
}

ice_uint ice_audio_init() {
	if (dsp_dma_buffer!=NULL) {
		ice_log((ice_char*)"DMA buffer is already initialized!");
		
		return 0;
	}
	
	{ //Check for Sound Blaster ports: 220, 230, 240, 250, 260 or 280
		unsigned int port_offset;
		
		for (
			port_offset=1; 
			port_offset<9; 
			port_offset++
		) {
			if (
				port_offset!=7 && 
				dsp_reset(0x200+(port_offset<<4))
			) {
				ice_char msg[64];
				sprintf(
					(char *)msg,
					"Detected Sound Blaster: %X",
					0x200+(port_offset<<4)
				);
				ice_log(msg);
				
				break;
			}
		}
		
		//Sound Blaster not detected
		if (port_offset==9) {
			ice_log((ice_char*)"Failed to detect Sound Blaster!");
			
			return 1;
		}
	}
	
	{ //Allocate sound buffer
		_go32_dpmi_seginfo temp_buffer; //Temporary pointer
		
		//Assign 32K to DMA Buffer
		dsp_dma_buffer=calloc(
			BUFFER_SIZE,
			sizeof(unsigned char)
		);
		
		//Assign 32K (2048 paragraphs) of DOS memory
		temp_buffer.size=BUFFER_SIZE/16;
		_go32_dpmi_allocate_dos_memory(&temp_buffer);

		//Calculate linear address
		dos_dma_buffer_offset=temp_buffer.rm_segment<<4;

		//Calculate page at start of buffer
		unsigned int page_1=dos_dma_buffer_offset>>16;

		//Calculate page at end of buffer}
		unsigned int page_2=(
			dos_dma_buffer_offset+
			(BUFFER_SIZE-1)
		)>>16;

		//Check to see if a page boundary is crossed
		if (page_1!=page_2) {
			ice_log((ice_char*)"Buffer overlaps page boundary! Reassigning...");
			//If so, assign another part of memory to the buffer
			dos_dma_buffer.size=BUFFER_SIZE/16;
			_go32_dpmi_allocate_dos_memory(&dos_dma_buffer);
			
			dos_dma_buffer_offset=dos_dma_buffer.rm_segment<<4;
			_go32_dpmi_free_dos_memory(&temp_buffer);
		} else {
			//otherwise, use the part we've already allocated
			dos_dma_buffer=temp_buffer;
		}
		
		//Clear DMA buffers
		memset(
			dsp_dma_buffer,
			127,
			BUFFER_SIZE
		);
		dosmemput(
			dsp_dma_buffer,
			BUFFER_SIZE,
			dos_dma_buffer_offset
		);
	}
	
	{ //Enable playback
		ice_log((ice_char*)"Initializing DSP...");
		
		outportb(0x0A,0x04+dsp_dma); //Disable DMA channel
		outportb(0x0C,1);            //Clear byte pointer
		outportb(0x0B,0x58+dsp_dma); //Set mode 0x48 = single | 0x58 = auto mode + channel
		
		dsp_write(0xD1); //DSP-command D1h - Enable speaker
		dsp_write(0x40); //DSP-command 40h - Set sample frequency
		dsp_write(210);  //Write time constant
		// 210 = 22 KHz | 225 = 32KHz | 233 = 44 KHz
		
		//Convert pointer to linear address
		unsigned int page   = dos_dma_buffer_offset>>16;    //Calculate page
		unsigned int offset = dos_dma_buffer_offset&0xFFFF; //Calculate offset in the page

		//Write the offset to the DMA controller
		outportb(dsp_dma<<1,offset&0xFF);
		outportb(dsp_dma<<1,offset>>8);

		if (dsp_dma==0) outportb(0x87,page);
		if (dsp_dma==1) outportb(0x83,page);
		if (dsp_dma==3) outportb(0x82,page);
		
		//Set the block length
		outportb((dsp_dma<<1)+1,(BUFFER_SIZE-1)&0xFF);
		outportb((dsp_dma<<1)+1,((BUFFER_SIZE-1)>>8)&0xFF);
		
		//DSP-command 48h - Set block length to 0x1FFF = 8 Kbyte
		dsp_write(0x48);
		dsp_write(0xFF);
		dsp_write(0x1F);
		
		//DSP-command 1Ch - Start auto-init playback
		dsp_write(0x1C);
		
		//Enable DMA channel
		outportb(0x0A,dsp_dma);
	}
	
	{ //Allocate slots
		samples = calloc(
			MAX_SAMPLES,
			sizeof(audio_sample)
		);
		
		sources = calloc(
			MAX_SOURCES,
			sizeof(audio_source)
		);
		
		audio_streams = calloc(
			MAX_STREAMS,
			sizeof(audio_stream)
		);
	}
	
	return 0;
}

void ice_audio_deinit() {
	ice_log((ice_char*)"Stopping DSP...");
	
	//Stop DMA-transfer
	dsp_write(0xD0);
	dsp_write(0xDA);
	
	//Free the memory allocated to the sound buffer
	if (dsp_dma_buffer!=NULL) {
		free(dsp_dma_buffer);
		dsp_dma_buffer=NULL;
	}
	
	ice_audio_sample_flush();
	ice_audio_source_flush();
	
	if (samples!=NULL) {
		free(samples);
		samples=NULL;
	}
	
	if (sources!=NULL) {
		free(sources);
		sources=NULL;
	}
	
	if (audio_streams!=NULL) {
		free(audio_streams);
		audio_streams=NULL;
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
			
			//Convert stereo down to mono
			for (int j=0; j<info.channels; j++) {
				pulse = (pulse+buffer[i+j]+32767)/2;
			}
			
			sample->data[data_index]=(unsigned char)(pulse/256);
			
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
		return 0;
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
		case ICE_AUDIO_STATE_PAUSED:
			source->state=ICE_AUDIO_STATE_PAUSED;
			break;
		case ICE_AUDIO_STATE_PLAYING:
			source->state=ICE_AUDIO_STATE_PLAYING;
			break;
		case ICE_AUDIO_STATE_LOOPING:
			source->state=ICE_AUDIO_STATE_LOOPING;
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
	if (audio_streams!=NULL) {
		for (ice_uint i=0; i<MAX_STREAMS; i++) {
			ice_audio_stream_delete(i);
		}
	}
}

ice_uint ice_audio_stream_load(
	ice_uint file_id
) {
	if (audio_streams==NULL) {
		ice_log((ice_char *)"Audio is not initialized!");
		
		return 0;
	}
	
	ice_uint      stream_id = 0;
	audio_stream *stream    = NULL;
	
	for (ice_uint i=0; i<MAX_STREAMS; i++) {
		if (audio_streams[i].ogg_stream==NULL) {
			stream_id = i;
			stream    = &audio_streams[i];
			
			break;
		}
	}
	
	if (stream==NULL) {
		ice_log((ice_char *)"Exceeded audio audio_streams limit");
		
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
		audio_streams==NULL || 
		stream_id>=MAX_STREAMS ||
		audio_streams[stream_id].ogg_stream==NULL
	) {
		return;
	}
	
	audio_stream *stream = &audio_streams[stream_id];
	
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
		audio_streams==NULL || 
		stream_id>=MAX_STREAMS ||
		audio_streams[stream_id].ogg_stream==NULL
	) {
		return 0;
	}
	
	audio_stream *stream = &audio_streams[stream_id];
	
	return (ice_real)stream->length/stream->rate;
}

ice_real ice_audio_stream_position_get(
	ice_uint stream_id
) {
	if (
		audio_streams==NULL || 
		stream_id>=MAX_STREAMS ||
		audio_streams[stream_id].ogg_stream==NULL
	) {
		return 0;
	}
	
	audio_stream *stream = &audio_streams[stream_id];
	
	return 0; //TODO
}

void ice_audio_stream_position_set(
	ice_uint stream_id,
	ice_real position
) {
	if (
		audio_streams==NULL || 
		stream_id>=MAX_STREAMS ||
		audio_streams[stream_id].ogg_stream==NULL
	) {
		return;
	}
	
	audio_stream *stream = &audio_streams[stream_id];
	
	//TODO
}

ice_uint ice_audio_stream_state_get(
	ice_uint stream_id
) {
	if (
		audio_streams==NULL || 
		stream_id>=MAX_STREAMS ||
		audio_streams[stream_id].ogg_stream==NULL
	) {
		return ICE_AUDIO_STATE_NONE;
	}
	
	audio_stream *stream = &audio_streams[stream_id];
	
	return stream->state;
}

void ice_audio_stream_state_set(
	ice_uint stream_id,
	ice_uint state
) {
	if (
		audio_streams==NULL || 
		stream_id>=MAX_STREAMS ||
		audio_streams[stream_id].ogg_stream==NULL
	) {
		return;
	}
	
	audio_stream *stream = &audio_streams[stream_id];
	
	switch(state) {
		case ICE_AUDIO_STATE_PAUSED:
			stream->state=ICE_AUDIO_STATE_PAUSED;
			break;
		case ICE_AUDIO_STATE_PLAYING:
			stream->state=ICE_AUDIO_STATE_PLAYING;
			break;
		case ICE_AUDIO_STATE_LOOPING:
			stream->state=ICE_AUDIO_STATE_LOOPING;
	}
}

ice_real ice_audio_stream_volume_get(
	ice_uint stream_id
) {
	if (
		audio_streams==NULL || 
		stream_id>=MAX_STREAMS ||
		audio_streams[stream_id].ogg_stream==NULL
	) {
		return 0;
	}
	
	audio_stream *stream = &audio_streams[stream_id];
	
	return (ice_real)stream->volume/255;
}

void ice_audio_stream_volume_set(
	ice_uint stream_id,
	ice_real volume
) {
	if (
		audio_streams==NULL || 
		stream_id>=MAX_STREAMS ||
		audio_streams[stream_id].ogg_stream==NULL
	) {
		return;
	}
	
	audio_stream *stream = &audio_streams[stream_id];
	
	volume = MAX(volume,0);
	volume = MIN(volume,1);
	
	stream->volume=(ice_char)(volume*255);
}