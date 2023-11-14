#include "ice.h"
#include "lib/iso/iso_vm.h"

#ifndef ISO_ICE_CLOCK_H
#define ISO_ICE_CLOCK_H

#define ISO_INT_CLOCK_GET 0x30

void iso_ice_clock_run(
	iso_vm *vm
) {
	if (vm->INT==ISO_INT_NONE)
		return;
	
	volatile iso_word A;
	
	switch(vm->INT) {
		case ISO_INT_CLOCK_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A.fp=ice_clock_get();
			
			iso_vm_push(vm,A.uint);
			
			break;
	}
}

#endif