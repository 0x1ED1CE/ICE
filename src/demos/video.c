#include "ice.h"

static ice_uint texture_stream_id;
static ice_uint audio_stream_id;

void ice_init() {
	texture_stream_id = ice_video_stream_load(0);
	audio_stream_id   = ice_audio_stream_load(0);
	
	ice_video_stream_state_set(
		texture_stream_id,
		ICE_VIDEO_STATE_PLAYING
	);
	ice_audio_stream_state_set(
		audio_stream_id,
		ICE_AUDIO_STATE_PLAYING
	);
}

void ice_deinit() {}

ice_uint ice_update(
	ice_real tick
) {
	ice_video_texture_rectangle_draw(
		texture_stream_id,
		0, 0,
		1, 1,
		0, 0,
		1, 1,
		1, 1, 1, 1
	);
	
	return ice_input_get(0,0x01)==1.0f;
}