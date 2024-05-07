#include "ice.h"

#include <SDL2/SDL.h>

#define TICKS_PER_SEC 1000;

unsigned int clock_init;

ice_uint ice_clock_init() {
	clock_init = SDL_GetTicks();

	return 0;
}

void ice_clock_deinit() {
	clock_init = 0;
}

ice_real ice_clock_get() {
	return (ice_real)(SDL_GetTicks()-clock_init)/TICKS_PER_SEC;
}