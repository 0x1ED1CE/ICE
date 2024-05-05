#include "ice.h"

void ice_init() {
	ice_uint sample = ice_audio_sample_load(0);
	ice_uint source = ice_audio_source_new();
	
	ice_audio_source_sample_set(
		source,
		sample
	);
	
	ice_audio_source_state_set(
		source,
		ICE_AUDIO_STATE_PLAYING
	);
}

void ice_deinit() {}

ice_uint ice_update(
	ice_real tick
) {
	return ice_input_get(0,0x01)==1.0f;
}