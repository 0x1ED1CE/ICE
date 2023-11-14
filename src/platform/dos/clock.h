#include <time.h>

uclock_t clock_init;

ice_uint ice_clock_init() {
	clock_init=uclock();
	
	return 0;
}

void ice_clock_deinit() {
	clock_init=0;
}

ice_float ice_clock_get() {
	return (ice_float)(uclock()-clock_init)/UCLOCKS_PER_SEC;
}