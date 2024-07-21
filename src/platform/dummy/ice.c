#include <stdio.h>
#include <stdlib.h>

#include "ice.h"

void ice_log(
	ice_char *message
) {
	printf(
		"[%.8X]: %s\n",
		(unsigned int)ice_clock_get(),
		(char *)message
	);
}

int main() {
	if (
		ice_clock_init() ||
		ice_audio_init() ||
		ice_video_init(0,0) ||
		ice_input_init()
	) {
		ice_clock_deinit();
		ice_audio_deinit();
		ice_video_deinit();
		ice_input_deinit();

		return -1;
	}

	ice_init();

	ice_real frame_start;
	ice_real frame_rate = 0;

	while (1) {
		frame_start = ice_clock_get();
		
		if (ice_update(frame_rate)) {
			break;
		}
		
		ice_audio_buffer(frame_rate);
		ice_video_buffer(frame_rate);
		
		frame_rate = ice_clock_get()-frame_start;
	}

	ice_deinit();

	ice_clock_deinit();
	ice_audio_deinit();
	ice_video_deinit();
	ice_input_deinit();

	return 0;
}