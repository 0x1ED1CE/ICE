#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "ice.h"

void ice_init() {
	ice_video_texture_triangle_draw(
		0, 2,
		159, 19,  1,
		39,  179, 1,
		279, 179, 1,
		0, 0,
		0, 0,
		0, 0,
		255, 0,   0,   255,
		0,   255, 0,   255,
		0,   0,   255, 255
	);
}

void ice_deinit() {}

ice_uint ice_update(
	ice_real tick
) {
	return ice_input_get(0,0x01)!=1.0f;
}