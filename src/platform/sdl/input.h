ice_uint ice_input_init() {
	return 0;
}

void ice_input_deinit() {
}

ice_char ice_input_poll(
	ice_uint *device_id,
	ice_uint *input_id,
	ice_real *input_state
) {
	return 0;
}

ice_real ice_input_get(
	ice_uint device_id,
	ice_uint input_id
) {
	if (device_id==0) {
		return (ice_real)SDL_GetKeyboardState(NULL)[input_id];
	}

	return 0;
}