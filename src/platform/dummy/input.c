#include "ice.h"

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
	return 0;
}