/*
MIT License

Copyright (c) 2023 Dice

Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>

#include "ice.h"
#include "lib/iso/iso_vm.h"

#ifndef ISO_ICE_AUX_H
#define ISO_ICE_AUX_H

void iso_aux_vm_debug_info(
	iso_vm *vm
) {
	{
		ice_char msg[256];
		sprintf(
			(char *)msg,
			"INT: %.8X\n"
			"PC:  %.8X\n"
			"SP:  %.8X\n\n"
			"CALL TRACE:\n",
			vm->INT,
			vm->PC,
			vm->SP
		);
		ice_log(msg);
	}
	
	iso_uint callback = vm->stack[0];
	iso_uint depth    = 0;
	
	while (
		callback!=0 &&
		callback<vm->stack_size &&
		depth<8
	) {
		{
			ice_char msg[16];
			sprintf(
				(char *)msg,
				"%.8X\n",
				(unsigned int)vm->stack[callback-2]
			);
			ice_log(msg);
		}
		
		callback=vm->stack[callback-1];
		depth++;
	}
	
	if (depth>=8) {
		ice_log((ice_char *)"...\n");
	}
}

void iso_aux_run(
	iso_vm *vm
) {
	if (vm->INT==ISO_INT_NONE)
		return;
	
	switch(vm->INT) {
		case ISO_INT_ILLEGAL_OPERATION:
			ice_log((ice_char *)"ILLEGAL OPERATION");
			iso_aux_vm_debug_info(vm);
			
			break;
		case ISO_INT_INVALID_JUMP:
			ice_log((ice_char *)"JUMP TO INVALID ADDRESS");
			iso_aux_vm_debug_info(vm);
			
			break;
		case ISO_INT_OUT_OF_BOUNDS:
			ice_log((ice_char *)"ACCESS OUT OF BOUNDS");
			iso_aux_vm_debug_info(vm);
			
			break;
		case ISO_INT_STACK_OVERFLOW:
			ice_log((ice_char *)"STACK OVERFLOW");
			iso_aux_vm_debug_info(vm);
			
			break;
		case ISO_INT_STACK_UNDERFLOW:
			ice_log((ice_char *)"STACK UNDERFLOW");
			iso_aux_vm_debug_info(vm);
			
			break;
		case ISO_INT_END_OF_PROGRAM:
			ice_log((ice_char *)"END OF PROGRAM");
			iso_aux_vm_debug_info(vm);
			
			break;
		default:
			ice_log((ice_char *)"UNHANDLED INTERRUPT");
			iso_aux_vm_debug_info(vm);
			
			break;
	}
}

#endif