#ifndef ICE_AUDIO_H
#define ICE_AUDIO_H

#define ICE_AUDIO_STATE_NONE    0
#define ICE_AUDIO_STATE_PAUSED  1
#define ICE_AUDIO_STATE_PLAYING 2
#define ICE_AUDIO_STATE_LOOP    3

ice_uint ice_audio_init();

void ice_audio_deinit();

void ice_audio_buffer(ice_real tick);

void ice_audio_sample_flush();

void ice_audio_source_flush();

ice_uint ice_audio_sample_load(
	ice_uint file_id
);

void ice_audio_sample_delete(
	ice_uint sample_id
);

ice_real ice_audio_sample_length_get(
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

ice_real ice_audio_source_position_get(
	ice_uint source_Id
);

void ice_audio_source_position_set(
	ice_uint source_id,
	ice_real position
);

ice_char ice_audio_source_state_get(
	ice_uint source_id
);

void ice_audio_source_state_set(
	ice_uint source_id,
	ice_char state
);

ice_real ice_audio_source_volume_get(
	ice_uint source_id
);

void ice_audio_source_volume_set(
	ice_uint source_id,
	ice_real volume
);

/* TODO
ice_uint ice_audio_stream_load(
	ice_uint file_id
);

ice_uint ice_audio_stream_delete(
	ice_uint stream_id
);

ice_real ice_audio_stream_length_get(
	ice_uint stream_id
);

ice_real ice_audio_stream_position_get(
	ice_uint stream_id
);

void ice_audio_stream_position_set(
	ice_uint stream_id,
	ice_real position
);

ice_char ice_audio_stream_state_get(
	ice_uint stream_id
);

void ice_audio_stream_state_set(
	ice_uint stream_id,
	ice_char state
);

ice_real ice_audio_stream_volume_get(
	ice_uint stream_id
);

void ice_audio_stream_volume_set(
	ice_uint stream_id,
	ice_real volume
);
*/

#endif