/*
Timing is handled by the audio module since PIT is used for audio processing.
*/

#include "ice.h"

volatile unsigned long clock_tick;

ice_uint ice_clock_init() {
	clock_tick=0;

	return 0;
}

void ice_clock_deinit() {}

ice_real ice_clock_get() {
	return (ice_real)clock_tick/22096;
}
