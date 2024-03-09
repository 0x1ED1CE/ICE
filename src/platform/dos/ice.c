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
		ice_video_init(640,480) ||
		ice_input_init()
	) {
		ice_clock_deinit();
		ice_audio_deinit();
		ice_video_deinit();
		ice_input_deinit();
		
		printf("An error occurred. See LOG.TXT for details.\n");
		
		return -1;
	}
	
	ice_init();
	
	ice_real frame_start;
	ice_real frame_rate = 0;
	
	while (1) {
		frame_start = ice_clock_get();
		
		ice_audio_buffer();
		ice_video_buffer();
		
		if (ice_update(frame_rate)) {
			break;
		}
		
		frame_rate = ice_clock_get()-frame_start;
	}
	
	ice_deinit();
	
	ice_clock_deinit();
	ice_audio_deinit();
	ice_video_deinit();
	ice_input_deinit();
	
	return 0;
}