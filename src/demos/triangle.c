#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "ice.h"

void ice_init() {
	ice_uint texture = ice_video_texture_load(0);
	
	ice_video_texture_triangle_draw(
		0, texture,
		0.5, 0.2, 0,
		0.2, 0.8, 0,
		0.8, 0.8, 0,
		0, 0,
		0, 1,
		1, 1,
		1, 0, 0, 1,
		0, 1, 0, 1,
		0, 0, 1, 1
	);
}

void ice_deinit() {}

ice_uint ice_update(
	ice_real tick
) {
	return ice_input_get(0,0x01)==1.0f;
}