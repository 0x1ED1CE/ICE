#include <stdio.h>
#include <stdlib.h>

#include "ice.h"

static FILE *log;

void ice_log(
	ice_char *message
) {
	if (log==NULL) return;

	fprintf(
		log,
		"[%.8X] %s\n",
		(unsigned int)ice_clock_get(),
		(char *)message
	);
}

int main() {
	log = fopen("log.txt","w");

	if (
		ice_clock_init() ||
		ice_audio_init() ||
		ice_video_init() ||
		ice_input_init()
	) {
		ice_clock_deinit();
		ice_audio_deinit();
		ice_video_deinit();
		ice_input_deinit();

		if (log!=NULL) fclose(log);

		printf("An error occurred. See LOG.TXT for details.\n");

		return -1;
	}

	ice_init();

	ice_real frame_start;
	ice_real frame_time = 0;

	while (1) {
		frame_start = ice_clock_get();

		ice_input_update();
		ice_video_update();
		ice_audio_update();

		if (ice_update(frame_time)) {
			break;
		}

		do {
			frame_time=ice_clock_get()-frame_start;
		} while (frame_time<0.01666f);
	}

	ice_deinit();

	ice_clock_deinit();
	ice_audio_deinit();
	ice_video_deinit();
	ice_input_deinit();

	if (log!=NULL) fclose(log);

	return 0;
}
