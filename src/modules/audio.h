#ifndef ICE_AUDIO_H
#define ICE_AUDIO_H

#define STATE_NONE    0
#define STATE_PAUSED  1
#define STATE_PLAYING 2
#define STATE_LOOP    3

ice_uint ice_audio_init();

void ice_audio_deinit();

void ice_audio_flush();

void ice_audio_buffer();

ice_uint ice_audio_sample_new(
	ice_uint file_id
);

void ice_audio_sample_delete(
	ice_uint sample_id
);

ice_uint ice_audio_sample_length_get(
	ice_uint sample_id
);

ice_uint ice_audio_source_new();

void ice_audio_source_delete(
	ice_uint source_id
);

ice_uint ice_audio_source_sample_get(
	ice_uint source_id
);

void ice_audio_source_sample_set(
	ice_uint source_id,
	ice_uint sample_id
);

ice_uint ice_audio_source_position_get(
	ice_uint source_Id
);

void ice_audio_source_position_set(
	ice_uint source_id,
	ice_uint position
);

ice_char ice_audio_source_state_get(
	ice_uint source_id
);

void ice_audio_source_state_set(
	ice_uint source_id,
	ice_char state
);

ice_uint ice_audio_source_volume_get(
	ice_uint source_id
);

void ice_audio_source_volume_set(
	ice_uint source_id,
	ice_uint volume
);

#endif