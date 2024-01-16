#include "ice.h"
#include "lib/iso/iso_vm.h"

#ifndef ISO_ICE_AUDIO_H
#define ISO_ICE_AUDIO_H

#define ISO_INT_AUDIO_BUFFER              0x3000
#define ISO_INT_AUDIO_FLUSH               0x3001
#define ISO_INT_AUDIO_SAMPLE_LOAD         0x3002
#define ISO_INT_AUDIO_SAMPLE_DELETE       0x3003
#define ISO_INT_AUDIO_SAMPLE_LENGTH_GET   0x3004
#define ISO_INT_AUDIO_SOURCE_NEW          0x3005
#define ISO_INT_AUDIO_SOURCE_DELETE       0x3006
#define ISO_INT_AUDIO_SOURCE_SAMPLE_GET   0x3007
#define ISO_INT_AUDIO_SOURCE_SAMPLE_SET   0x3008
#define ISO_INT_AUDIO_SOURCE_STATE_GET    0x3009
#define ISO_INT_AUDIO_SOURCE_STATE_SET    0x300A
#define ISO_INT_AUDIO_SOURCE_POSITION_GET 0x300B
#define ISO_INT_AUDIO_SOURCE_POSITION_SET 0x300C
#define ISO_INT_AUDIO_SOURCE_VOLUME_GET   0x300D
#define ISO_INT_AUDIO_SOURCE_VOLUME_SET   0x300E

void iso_ice_audio_run(
	iso_vm *vm
) {
	if (vm->INT==ISO_INT_NONE)
		return;
	
	iso_uint A,B;
	
	switch(vm->INT) {
		case ISO_INT_AUDIO_BUFFER:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			ice_audio_buffer();
			
			break;
		case ISO_INT_AUDIO_FLUSH:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			ice_audio_flush();
			
			break;
		case ISO_INT_AUDIO_SAMPLE_LOAD:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = (iso_uint)ice_audio_sample_load((ice_uint)A);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_AUDIO_SAMPLE_DELETE:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			ice_audio_sample_delete((ice_uint)A);
			
			break;
		case ISO_INT_AUDIO_SAMPLE_LENGTH_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = ice_audio_sample_length_get((ice_uint)A);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_AUDIO_SOURCE_NEW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = (iso_uint)ice_audio_source_new();
			
			iso_vm_push(vm,A);
			
			break;
		case ISO_INT_AUDIO_SOURCE_DELETE:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			ice_audio_source_delete((ice_uint)A);
			
			break;
		case ISO_INT_AUDIO_SOURCE_SAMPLE_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = ice_audio_source_sample_get((ice_uint)A);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_AUDIO_SOURCE_SAMPLE_SET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			B = iso_vm_pop(vm);
			A = iso_vm_pop(vm);
			
			ice_audio_source_sample_set(
				(ice_uint)A,
				(ice_uint)B
			);
			
			break;
		case ISO_INT_AUDIO_SOURCE_STATE_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = ice_audio_source_state_get((ice_uint)A);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_AUDIO_SOURCE_STATE_SET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			B = iso_vm_pop(vm);
			A = iso_vm_pop(vm);
			
			ice_audio_source_state_set(
				(ice_uint)A,
				(ice_uint)B
			);
			
			break;
		case ISO_INT_AUDIO_SOURCE_POSITION_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = ice_audio_source_position_get((ice_uint)A);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_AUDIO_SOURCE_POSITION_SET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = iso_vm_pop(vm);
			
			ice_audio_source_position_set(
				(ice_uint)A,
				(ice_uint)B
			);
			
			break;
		case ISO_INT_AUDIO_SOURCE_VOLUME_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = ice_audio_source_volume_get((ice_uint)A);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_AUDIO_SOURCE_VOLUME_SET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = iso_vm_pop(vm);
			
			ice_audio_source_volume_set(
				(ice_uint)A,
				(ice_uint)B
			);
			
			break;
	}
}

#endif