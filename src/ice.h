#ifndef ICE_H
#define ICE_H

#define ICE_VERSION 4

typedef unsigned char ice_char;
typedef unsigned int  ice_uint;
typedef signed int    ice_sint;
typedef float         ice_real;

#include "modules/clock.h"
#include "modules/video.h"
#include "modules/audio.h"
#include "modules/input.h"
#include "modules/cache.h"

void ice_log(
	ice_char *message
);

void ice_init();

void ice_deinit();

ice_uint ice_update(
	ice_real tick
);

#endif