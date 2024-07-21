#include <time.h>

#include "ice.h"

uclock_t clock_init;

ice_uint ice_clock_init() {
	clock_init=clock();
	
	return 0;
}

void ice_clock_deinit() {
	clock_init=0;
}

ice_real ice_clock_get() {
	return (ice_real)(uclock()-clock_init)/CLOCKS_PER_SEC;
}