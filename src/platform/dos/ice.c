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

int main() {
	if (
		ice_clock_init() ||
		ice_audio_init() ||
		ice_video_init() ||
		ice_input_init()
	) {
		ice_deinit();
		
		printf("An error occurred. See LOG.TXT for more info.\n");
		
		return -1;
	}
	
	ice_init();
	
	ice_real clock;
	
	do {
		clock = ice_clock_get();
		
		ice_audio_buffer();
		ice_video_buffer();
		
	} while (ice_update(ice_clock_get()-clock));
	
	ice_deinit();
	
	ice_clock_deinit();
	ice_audio_deinit();
	ice_video_deinit();
	ice_input_deinit();
	
	return 0;
}