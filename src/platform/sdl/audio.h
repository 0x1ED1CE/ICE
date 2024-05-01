#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define BUFFER_SIZE 2048

#define MAX_SAMPLES 32
#define MAX_SOURCES 64

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

audio_sample *samples = NULL;
audio_source *sources = NULL;

static SDL_AudioSpec audio_spec;

void sdl_audio_buffer(
	void *userdata,
	Uint8 *stream,
	int len
) {
	unsigned int mix_a;
	unsigned int mix_b;
	unsigned int mix_c;

	audio_sample *sample;
	audio_source *source;

	for (unsigned int i=0; i<len; i++) {
		stream[i]=127;

		for (unsigned int j=0; j<MAX_SOURCES; j++) {
			source = &sources[j];
			sample = &samples[source->sample_id];

			if (
				sample->data!=NULL &&
				source->state>ICE_SOURCE_STATE_PAUSED
			) {
				mix_a = (unsigned int)stream[i];
				mix_b = (unsigned int)sample->data[source->position];
				mix_c = (mix_a+mix_b)/2;

				stream[i]=(Uint8)mix_c;

				source->position += 1;
				source->position %= sample->length;

				if (
					source->position==0 &&
					source->state==ICE_SOURCE_STATE_PLAYING
				) {
					source->state=ICE_SOURCE_STATE_PAUSED;
				}
			}
		}
	};
}

ice_uint ice_audio_init() {
	audio_spec.freq     = 22050;
	audio_spec.format   = AUDIO_U8;
	audio_spec.channels = 1;
	audio_spec.samples  = BUFFER_SIZE;
	audio_spec.callback = sdl_audio_buffer;
	audio_spec.userdata = NULL;

	if (SDL_OpenAudio(
		&audio_spec,
		NULL
	)!=0) {
		char error_msg[256];

		sprintf(
			error_msg,
			"Failed to initialize audio: %s",
			SDL_GetError()
		);

		ice_log((ice_char *)error_msg);

		return 1;
	}

	samples=(audio_sample *)calloc(
		MAX_SAMPLES,
		sizeof(audio_sample)
	);
	sources=(audio_source *)calloc(
		MAX_SOURCES,
		sizeof(audio_source)
	);

	SDL_PauseAudio(0);

    return 0;
}

void ice_audio_deinit() {
	SDL_CloseAudio();

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
}

void ice_audio_buffer() {
}

void ice_audio_sample_flush() {
	if (samples!=NULL) {
		for (ice_uint i=0; i<MAX_SAMPLES; i++) {
			ice_audio_sample_delete(i);
		}
	}
}

void ice_audio_source_flush() {
	if (sources!=NULL) {
		for (ice_uint i=0; i<MAX_SOURCES; i++) {
			ice_audio_source_delete(i);
		}
	}
}

ice_uint ice_audio_sample_load(
	ice_uint file_id
) {
    if (samples==NULL) {
		return 0;
	}

	unsigned int sample_id = 0;
	audio_sample *sample   = NULL;

	for (unsigned int i=0; i<MAX_SAMPLES; i++) {
		if (samples[i].data==NULL) {
			sample_id = i;
			sample    = &samples[i];

			break;
		}
	}

	if (sample==NULL) {
		return 0;
	}

	FILE *wav_file;
	char filename[32];
	sprintf(filename,"%u.wav",(unsigned int)file_id);

	SDL_AudioSpec sample_spec;

	if (SDL_LoadWAV(
		filename,
		&audio_spec,
		(Uint8 **)&sample->data,
		&sample->length
	)==NULL) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to open audio sample: %d",
			file_id
		);
		ice_log(msg);

		return 01;
	}

	sample->rate = sample_spec.freq;

	if (sample->data==NULL) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to allocate audio sample: %d",
			file_id
		);
		ice_log(msg);

		sample->length = 0;
		sample->rate   = 0;

		return 0;
	}

	return (ice_uint)sample_id;
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

	SDL_FreeWAV(sample->data);

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

ice_uint ice_audio_source_new() {
	if (sources==NULL) {
		return 0;
	}

	for (unsigned int i=0; i<MAX_SOURCES; i++) {
		if (sources[i].state==ICE_SOURCE_STATE_NONE) {
			audio_source *source=&sources[i];

			source->state     = ICE_SOURCE_STATE_PAUSED;
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
		sources[source_id].state==ICE_SOURCE_STATE_NONE
	) {
		return;
	}

	audio_source *source=&sources[source_id];

	source->state     = ICE_SOURCE_STATE_NONE;
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
		sources[source_id].state==ICE_SOURCE_STATE_NONE
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
		sources[source_id].state==ICE_SOURCE_STATE_NONE
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
		sources[source_id].state==ICE_SOURCE_STATE_NONE
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
		sources[source_id].state==ICE_SOURCE_STATE_NONE
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

ice_char ice_audio_source_state_get(
	ice_uint source_id
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_SOURCE_STATE_NONE
	) {
		return 0;
	}

	return (ice_uint)sources[source_id].state;
}

void ice_audio_source_state_set(
	ice_uint source_id,
	ice_char state
) {
	if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_SOURCE_STATE_NONE
	) {
		return;
	}

	audio_source *source=&sources[source_id];

	switch(state) {
		case ICE_SOURCE_STATE_PAUSED:
			source->state=ICE_SOURCE_STATE_PAUSED;
			break;
		case ICE_SOURCE_STATE_PLAYING:
			source->state=ICE_SOURCE_STATE_PLAYING;
			break;
		case ICE_SOURCE_STATE_LOOP:
			source->state=ICE_SOURCE_STATE_LOOP;
	}
}

ice_real ice_audio_source_volume_get(
	ice_uint source_id
) {
   if (
		sources==NULL ||
		source_id>=MAX_SOURCES ||
		sources[source_id].state==ICE_SOURCE_STATE_NONE
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
		sources[source_id].state==ICE_SOURCE_STATE_NONE
	) {
		return;
	}

	volume = MAX(volume,0);
	volume = MIN(volume,1);

	sources[source_id].volume=(ice_char)(volume*255);
}