#include <stdio.h>
#include <stdlib.h>

#include "ice.h"

#include "lib/iso/iso_vm.h"
#include "lib/iso/iso_aux.h"

#include "api/iso/iso_ice_audio.h"
#include "api/iso/iso_ice_video.h"
#include "api/iso/iso_ice_clock.h"

#include "game.h"

#define ISO_STACK_SIZE 0xFFFF

int main() {
	iso_vm vm;
	vm.INT          = 0;
	vm.PC           = 0;
	vm.SP           = 0;
	vm.program_size = sizeof(GAME_H);
	vm.program      = GAME_H;
	vm.stack_size   = ISO_STACK_SIZE;
	vm.stack        = (iso_uint *)calloc(
		ISO_STACK_SIZE,
		sizeof(iso_uint)
	);
	
	if (vm.stack==NULL) {
		ice_log((ice_char*)"FAILED TO INITIALIZE MEMORY\n");
		
		return -1;
	}
	
	switch(ice_init()) {
		case 0:
			break;
		case ICE_INIT_ERROR_CLOCK:
			free(vm.stack);
			ice_deinit();
			ice_log((ice_char*)"FAILED TO INITIALIZE CLOCK SUBSYSTEM\n");
			return -2;
		case ICE_INIT_ERROR_AUDIO:
			free(vm.stack);
			ice_deinit();
			ice_log((ice_char*)"FAILED TO INITIALIZE AUDIO SUBSYSTEM\n");
			return -3;
		case ICE_INIT_ERROR_VIDEO:
			free(vm.stack);
			ice_deinit();
			ice_log((ice_char*)"FAILED TO INITIALIZE VIDEO SUBSYSTEM\n");
			return -4;
		case ICE_INIT_ERROR_INPUT:
			free(vm.stack);
			ice_deinit();
			ice_log((ice_char*)"FAILED TO INITIALIZE INPUT SUBSYSTEM\n");
			return -5;
		case ICE_INIT_ERROR_CACHE:
			free(vm.stack);
			ice_deinit();
			ice_log((ice_char*)"FAILED TO INITIALIZE CACHE SUBSYSTEM\n");
			return -6;
		default:
			free(vm.stack);
			ice_deinit();
			ice_log((ice_char*)"UNKNOWN ERROR DURING INITIALIZATION\n");
			return -7;
	}
	
	do {
		iso_vm_run(&vm,0);
		iso_ice_audio_run(&vm);
		iso_ice_video_run(&vm);
		iso_ice_clock_run(&vm);
	} while (vm.INT==0);
	
	ice_deinit();
	free(vm.stack);
	iso_aux_run(&vm);
	
	return (int)vm.INT;
}