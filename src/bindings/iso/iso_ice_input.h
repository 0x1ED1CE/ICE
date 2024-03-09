#include "ice.h"
#include "lib/iso/iso_vm.h"

#ifndef ISO_ICE_INPUT_H
#define ISO_ICE_INPUT_H

#define ISO_INT_INPUT_GET  0x4000
#define ISO_INT_INPUT_POLL 0x4001

void iso_ice_input_run(
	iso_vm *vm
) {
	if (vm->INT==ISO_INT_NONE)
		return;
	
	switch(vm->INT) {
		case ISO_INT_INPUT_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				iso_uint B = iso_vm_pop(vm);
				iso_uint A = iso_vm_pop(vm);
				iso_word C;
				
				C.real = (iso_real)ice_input_get(
					(ice_uint)A,
					(ice_uint)B
				);
			
				iso_vm_push(vm,C.uint);
			}
			
			break;
		case ISO_INT_INPUT_POLL:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			{
				ice_uint A;
				ice_uint B;
				ice_real C;
				
				ice_input_poll(&A,&B,&C);
			
				
			}
			
			break;
	}
}

#endif