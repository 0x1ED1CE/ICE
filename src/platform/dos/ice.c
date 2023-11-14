#include <stdio.h>

#include "ice.h"

#include "audio.h"
#include "video.h"
#include "clock.h"

void ice_log(
	ice_char *message
) {
	FILE *log = fopen("log.txt","a");
	
	fprintf(
		log,
		"[%.8X]: %s\n",
		(unsigned int)ice_clock_get(),
		(char *)message
	);
	
	fclose(log);
}

ice_uint ice_init() {
	if (ice_clock_init()) {
		return ICE_INIT_ERROR_CLOCK;
	}
	if (ice_audio_init()) {
		return ICE_INIT_ERROR_AUDIO;
	}
	if (ice_video_init()) {
		return ICE_INIT_ERROR_VIDEO;
	}
	
	return 0;
}

void ice_deinit() {
	ice_clock_deinit();
	ice_audio_deinit();
	ice_video_deinit();
}