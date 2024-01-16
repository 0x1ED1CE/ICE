#ifndef ICE_INPUT_H
#define ICE_INPUT_H

ice_uint ice_input_init();

void ice_input_deinit();

ice_char ice_input_poll(
	ice_uint *device_id,
	ice_uint *input_id,
	ice_real *input_state
);

ice_real ice_input_get(
	ice_uint device_id,
	ice_uint input_id
);

#endif