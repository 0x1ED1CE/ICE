#include "ice.h"
#include "lib/iso/iso_vm.h"

#ifndef ISO_ICE_AUDIO_H
#define ISO_ICE_AUDIO_H

#define ISO_INT_AUDIO_SAMPLE_FLUSH        0x3000
#define ISO_INT_AUDIO_SAMPLE_LOAD         0x3001
#define ISO_INT_AUDIO_SAMPLE_DELETE       0x3002
#define ISO_INT_AUDIO_SAMPLE_LENGTH_GET   0x3003
#define ISO_INT_AUDIO_SOURCE_FLUSH        0x3004
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
	
	switch(vm->INT) {
		case ISO_INT_AUDIO_SAMPLE_FLUSH:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			ice_audio_sample_flush();
			
			break;
		case ISO_INT_AUDIO_SAMPLE_LOAD:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_uint B = ice_audio_sample_load(A);
			
				iso_vm_push(vm,(iso_uint)B);
			}
			
			break;
		case ISO_INT_AUDIO_SAMPLE_DELETE:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_audio_sample_delete(A);
			}
			
			break;
		case ISO_INT_AUDIO_SAMPLE_LENGTH_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_uint B = ice_audio_sample_length_get(A);
				
				iso_vm_push(vm,(iso_uint)B);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_FLUSH:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			ice_audio_source_flush();
			
			break;
		case ISO_INT_AUDIO_SOURCE_NEW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = ice_audio_source_new();
				
				iso_vm_push(vm,(iso_uint)A);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_DELETE:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_audio_source_delete(A);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_SAMPLE_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_uint B = ice_audio_source_sample_get(A);
			
				iso_vm_push(vm,(iso_uint)B);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_SAMPLE_SET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_uint B = (ice_uint)iso_vm_pop(vm);
				
				ice_audio_source_sample_set(A,B);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_STATE_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_uint B = ice_audio_source_state_get(A);
				
				iso_vm_push(vm,(iso_uint)B);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_STATE_SET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_uint B = (ice_uint)iso_vm_pop(vm);
				
				ice_audio_source_state_set(A,B);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_POSITION_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_word B;
				
				B.real = ice_audio_source_position_get(A);
				
				iso_vm_push(vm,(iso_uint)B.uint);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_POSITION_SET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_word B;
				
				B.uint = (ice_uint)iso_vm_pop(vm);
				
				ice_audio_source_position_set(A,B.real);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_VOLUME_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_word B;
				
				B.real = ice_audio_source_volume_get(A);
				
				iso_vm_push(vm,(iso_uint)B.uint);
			}
			
			break;
		case ISO_INT_AUDIO_SOURCE_VOLUME_SET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A = (ice_uint)iso_vm_pop(vm);
				ice_word B;
				
				B.uint = (ice_uint)iso_vm_pop(vm);
				
				ice_audio_source_volume_set(A,B.real);
			}
			
			break;
	}
}

#endif