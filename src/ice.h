#ifndef ICE_H
#define ICE_H

#define ICE_VERSION 2

#define ICE_INIT_ERROR_CLOCK 0x01
#define ICE_INIT_ERROR_AUDIO 0x02
#define ICE_INIT_ERROR_VIDEO 0x03
#define ICE_INIT_ERROR_INPUT 0x04
#define ICE_INIT_ERROR_CACHE 0x05

typedef unsigned char ice_char;
typedef unsigned int  ice_uint;
typedef float         ice_float;

#include "modules/video.h"
#include "modules/audio.h"
#include "modules/clock.h"
#include "modules/input.h"
#include "modules/cache.h"

void ice_log(
	ice_char *message
);

ice_uint ice_init();

void ice_deinit();

#endif