#include "ice.h"

#include <SDL2/SDL.h>

ice_uint ice_input_init() {
	return 0;
}

void ice_input_deinit() {
}

void ice_input_update() {
}

ice_char ice_input_poll(
	ice_uint *device_id,
	ice_uint *sensor_id,
	ice_real *sensor_state
) {
	return 0;
}

ice_real ice_input_get(
	ice_uint device_id,
	ice_uint sensor_id
) {
	if (device_id==0) {
		return (ice_real)SDL_GetKeyboardState(NULL)[sensor_id];
	}

	return 0;
}