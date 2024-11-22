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
	ice_real *input_state
) {
	*device_id   = 0;
	*sensor_id   = 0;
	*input_state = 0;

	//TODO

	return 0;
}

ice_real ice_input_get(
	ice_uint device_id,
	ice_uint sensor_id
) {
	switch(device_id) {
		case ICE_INPUT_DEVICE_KEYBOARD:
			return (ice_real)SDL_GetKeyboardState(NULL)[sensor_id];

			break;
	}

	return 0;
}