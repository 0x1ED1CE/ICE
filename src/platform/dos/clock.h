#include <time.h>

uclock_t clock_init;

ice_uint ice_clock_init() {
	clock_init=uclock();
	
	return 0;
}

void ice_clock_deinit() {
	clock_init=0;
}

ice_real ice_clock_get() {
	return (ice_real)(uclock()-clock_init)/UCLOCKS_PER_SEC;
}