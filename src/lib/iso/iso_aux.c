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

#include "iso_aux.h"

void iso_aux_vm_debug_info(
	iso_vm *vm
) {
	printf(
		"INT: %.8X\n"
		"PC:  %.8X\n"
		"SP:  %.8X\n\n"
		"CALL TRACE:\n",
		vm->INT,
		vm->PC,
		vm->SP
	);
	
	iso_uint callback=vm->stack[0];
	
	while (
		callback!=0 && 
		callback<vm->stack_size
	) {
		printf("%.8X\n",vm->stack[callback-1]);
		callback=vm->stack[callback];
	}
}

void iso_aux_run(
	iso_vm *vm
) {
	if (vm->INT==ISO_INT_NONE)
		return;
	
	switch(vm->INT) {
		case ISO_INT_ILLEGAL_OPERATION:
			printf("ILLEGAL OPERATION\n\n");
			iso_aux_vm_debug_info(vm);
			
			break;
		case ISO_INT_INVALID_JUMP:
			printf("JUMP TO INVALID ADDRESS\n\n");
			iso_aux_vm_debug_info(vm);
			
			break;
		case ISO_INT_OUT_OF_BOUNDS:
			printf("MEMORY OUT OF BOUNDS\n\n");
			iso_aux_vm_debug_info(vm);
			
			break;
		case ISO_INT_STACK_OVERFLOW:
			printf("STACK OVERFLOW\n\n");
			iso_aux_vm_debug_info(vm);
			
			break;
		case ISO_INT_STACK_UNDERFLOW:
			printf("STACK UNDERFLOW\n\n");
			iso_aux_vm_debug_info(vm);
			
			break;
		default:
			printf("UNHANDLED INTERRUPT\n\n");
			iso_aux_vm_debug_info(vm);
	}
}