#include <stdio.h>
#include <stdlib.h>
//#include <bios.h>

#include "ice.h"

#define DEBUG_PORT 1

void ice_log(
	ice_char *message
) {
	/*
	for (
		ice_uint i=0;
		message[i];
		i++
	) {
		_bios_serialcom(
			_COM_SEND,
			DEBUG_PORT,
			(unsigned)message[i]
		);
	}
	*/
	
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
	/*
	_bios_serialcom(
		_COM_INIT,
		DEBUG_PORT,
		_COM_9600|
		_COM_NOPARITY|
		_COM_STOP1|
		_COM_CHR8
	);
	*/
	
	if (
		ice_clock_init() ||
		ice_audio_init() ||
		ice_video_init(320,200) ||
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
	ice_real frame_time = 0;
	
	while (1) {
		frame_start = ice_clock_get();
		
		if (ice_update(frame_time)) {
			break;
		}
		
		ice_audio_buffer(frame_time);
		ice_video_buffer(frame_time);
		
		frame_time = ice_clock_get()-frame_start;
	}
	
	ice_deinit();
	
	ice_clock_deinit();
	ice_audio_deinit();
	ice_video_deinit();
	ice_input_deinit();
	
	return 0;
}
