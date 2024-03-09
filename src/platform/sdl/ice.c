#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

SDL_Event sdl_event;

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

int main(
	int argc,
	char *argv[]
) {
	if (SDL_Init(
		SDL_INIT_TIMER ||
		SDL_INIT_VIDEO ||
		SDL_INIT_AUDIO ||
		SDL_INIT_EVENTS ||
		SDL_INIT_GAMECONTROLLER
	)<0){
		char error_msg[256];

		sprintf(
			error_msg,
			"Failed to initialize SDL: %s",
			SDL_GetError()
		);

		ice_log((ice_char *)error_msg);

		SDL_Quit();

		return -1;
	}

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

		SDL_Quit();

		return -1;
	}

	ice_init();

	ice_real frame_start;
	ice_real frame_time = 0;

	while (1) {
		frame_start = ice_clock_get();

		while (SDL_PollEvent(&sdl_event)) {
			if (sdl_event.type==SDL_QUIT) {
				ice_deinit();

				ice_clock_deinit();
				ice_audio_deinit();
				ice_video_deinit();
				ice_input_deinit();

				SDL_Quit();
			}
		}

		if (ice_update(frame_time)) {
			break;
		}

		frame_time = ice_clock_get()-frame_start;
	}

	ice_deinit();

	ice_clock_deinit();
	ice_audio_deinit();
	ice_video_deinit();
	ice_input_deinit();

	SDL_Quit();

	return 0;
}