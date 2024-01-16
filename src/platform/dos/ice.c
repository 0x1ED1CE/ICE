#include <stdio.h>
#include <stdlib.h>

#include "ice.h"

#include "clock.h"
#include "video.h"
#include "audio.h"
#include "input.h"
#include "cache.h"

void ice_log(
	ice_char *message
) {
	FILE *log = fopen("log.txt","a");
	
	if (log==NULL) {
		return;
	}
	
	fprintf(
		log,
		"[%.8X]: %s\n",
		(unsigned int)ice_clock_get(),
		(char *)message
	);
	
	fclose(log);
}

ice_uint ice_init() {
	ice_log((ice_char *)"Initializing...");
	
	if (ice_clock_init()) {
		return ICE_INIT_ERROR_CLOCK;
	}
	if (ice_audio_init()) {
		return ICE_INIT_ERROR_AUDIO;
	}
	if (ice_video_init()) {
		return ICE_INIT_ERROR_VIDEO;
	}
	if (ice_input_init()) {
		return ICE_INIT_ERROR_INPUT;
	}
	
	ice_log((ice_char *)"Initialization done.");
	
	return 0;
}

void ice_deinit() {
	ice_log((ice_char *)"Exiting...");
	
	ice_clock_deinit();
	ice_audio_deinit();
	ice_video_deinit();
	ice_input_deinit();
}