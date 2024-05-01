#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "ice.h"

static ice_uint texture_stream_id;

void ice_init() {
	texture_stream_id = ice_video_stream_load(0);
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