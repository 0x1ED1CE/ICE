#include "ice.h"

void ice_init() {
	//TODO
}

void ice_deinit() {}

ice_uint ice_update(
	ice_real tick
) {
	return ice_input_get(0,0x01)==1.0f;
}