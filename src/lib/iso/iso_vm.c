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

#include <math.h>

#include "iso_vm.h"

void iso_vm_interrupt(
	iso_vm *vm,
	iso_uint interrupt
) {
	vm->INT=interrupt;
}

iso_char iso_vm_fetch(
	iso_vm *vm
) {
	if (vm->INT)
		return 0;
	
	iso_uint  PC           = vm->PC;
	iso_uint  program_size = vm->program_size;
	iso_char *program      = vm->program;
	
	if (PC==program_size) {
		iso_vm_interrupt(
			vm,
			ISO_INT_END_OF_PROGRAM
		);
		
		return 0;
	}
	
	vm->PC = PC+1;
	
	return program[PC];
}

void iso_vm_push(
	iso_vm *vm,
	iso_uint value
) {
	if (vm->INT)
		return;
	
	iso_uint  SP         = vm->SP;
	iso_uint  stack_size = vm->stack_size;
	iso_uint *stack      = vm->stack;
	
	if (SP>=stack_size) {
		iso_vm_interrupt(
			vm,
			ISO_INT_STACK_OVERFLOW
		);
		
		return;
	}
	
	vm->SP    = SP+1;
	stack[SP] = value;
}

iso_uint iso_vm_pop(
	iso_vm *vm
) {
	if (vm->INT)
		return 0;
	
	iso_uint  SP    = vm->SP;
	iso_uint *stack = vm->stack;
	
	if (SP==0) {
		iso_vm_interrupt(
			vm,
			ISO_INT_STACK_UNDERFLOW
		);
		
		return 0;
	}
	
	vm->SP=SP-1;
	
	return stack[vm->SP];
}

void iso_vm_hop(
	iso_vm *vm,
	iso_uint address
) {
	if (vm->INT)
		return;
	
	iso_uint stack_size = vm->stack_size;
	
	if (address>=stack_size) {
		iso_vm_interrupt(
			vm,
			ISO_INT_STACK_OVERFLOW
		);
		
		return;
	}
	
	vm->SP = address;
}

void iso_vm_set(
	iso_vm *vm,
	iso_uint address,
	iso_uint value
) {
	if (vm->INT)
		return;
	
	iso_uint *stack      = vm->stack;
	iso_uint  stack_size = vm->stack_size;
	
	if (address>=stack_size) {
		iso_vm_interrupt(
			vm,
			ISO_INT_OUT_OF_BOUNDS
		);
		
		return;
	}
	
	stack[address]=value;
}

iso_uint iso_vm_get(
	iso_vm *vm,
	iso_uint address
) {
	if (vm->INT)
		return 0;
	
	iso_uint *stack      = vm->stack;
	iso_uint  stack_size = vm->stack_size;
	
	if (address>=stack_size) {
		iso_vm_interrupt(
			vm,
			ISO_INT_OUT_OF_BOUNDS
		);
		
		return 0;
	}
	
	return stack[address];
}

void iso_vm_jump(
	iso_vm *vm,
	iso_uint address
) {
	if (vm->INT)
		return;
	
	if (address>=vm->program_size) {
		iso_vm_interrupt(
			vm,
			ISO_INT_INVALID_JUMP
		);
		
		return;
	}
	
	vm->PC=address;
}

void iso_vm_run(
	iso_vm *vm,
	iso_uint step
) {
	if (vm->INT)
		return;
	
	/* Registers */
	
	iso_uint *INT = &vm->INT;
	iso_uint *SP  = &vm->SP;
	
	/* Working variables */
	
	iso_uint A,B,C;
	iso_char D,E;
	
	volatile iso_word G = {0};
	volatile iso_word H = {0};
	volatile iso_word I = {0};
	
	iso_char OP;
	
	do {
		switch(OP=iso_vm_fetch(vm)) {
			case ISO_OP_INT:
				A = iso_vm_pop(vm);
				
				iso_vm_interrupt(vm,A);
				 
				break;
			case ISO_OP_JMP:
				A = iso_vm_pop(vm);
				
				iso_vm_jump(vm,A);
				
				break;
			case ISO_OP_JMC:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				if (A)
					iso_vm_jump(vm,B);
				
				break;
			case ISO_OP_CEQ:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A==B);
				
				break;
			case ISO_OP_CNE:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A!=B);
				
				break;
			case ISO_OP_CLS:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A<B);
				
				break;
			case ISO_OP_CLE:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A<=B);
				
				break;
			case ISO_OP_HOP:
				A = iso_vm_pop(vm);
				
				iso_vm_hop(vm,A);
				
				break;
			case ISO_OP_POS:
				iso_vm_push(vm,*SP);
				
				break;
			case ISO_OP_DUP:
				A = iso_vm_get(vm,*SP-1);
				
				iso_vm_push(vm,A);
				
				break;
			case ISO_OP_POP:
				iso_vm_pop(vm);
				
				break;
			case ISO_OP_SET:
				A = iso_vm_pop(vm);
				B = iso_vm_pop(vm);
				
				iso_vm_set(vm,A,B);
				
				break;
			case ISO_OP_GET:
				A = iso_vm_pop(vm);
				B = iso_vm_get(vm,A);
				
				iso_vm_push(vm,B);
				
				break;
			case ISO_OP_ADD:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A+B);
				
				break;
			case ISO_OP_SUB:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A-B);
				
				break;
			case ISO_OP_MUL:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A*B);
				
				break;
			case ISO_OP_DIV:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A/B);
				
				break;
			case ISO_OP_POW:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				C = 1;
				
				for (; B>0; B--) {
					C=C*A;
				}
				
				iso_vm_push(vm,C);
				
				break;
			case ISO_OP_MOD:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A%B);
				
				break;
			case ISO_OP_NOT:
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,~A);
				
				break;
			case ISO_OP_AND:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A&B);
				
				break;
			case ISO_OP_BOR:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A|B);
				
				break;
			case ISO_OP_XOR:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A^B);
				
				break;
			case ISO_OP_LSH:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A<<B);
				
				break;
			case ISO_OP_RSH:
				B = iso_vm_pop(vm);
				A = iso_vm_pop(vm);
				
				iso_vm_push(vm,A>>B);
				
				break;
			case ISO_OP_FTU:
				G.uint = iso_vm_pop(vm);
				H.uint = (iso_uint)G.fp;
				
				iso_vm_push(vm,H.uint);
				
				break;
			case ISO_OP_UTF:
				G.uint = iso_vm_pop(vm);
				H.fp   = (iso_float)G.uint;
				
				iso_vm_push(vm,H.uint);
				
				break;
			case ISO_OP_FEQ:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				
				iso_vm_push(vm,G.fp==H.fp);
				
				break;
			case ISO_OP_FNE:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				
				iso_vm_push(vm,G.fp!=H.fp);
				
				break;
			case ISO_OP_FLS:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				
				iso_vm_push(vm,G.fp<H.fp);
				
				break;
			case ISO_OP_FLE:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				
				iso_vm_push(vm,G.fp<=H.fp);
				
				break;
			case ISO_OP_FAD:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				I.fp   = G.fp+H.fp;
				
				iso_vm_push(vm,I.uint);
				
				break;
			case ISO_OP_FSU:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				I.fp   = G.fp-H.fp;
				
				iso_vm_push(vm,I.uint);
				
				break;
			case ISO_OP_FMU:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				I.fp   = G.fp*H.fp;
				
				iso_vm_push(vm,I.uint);
				
				break;
			case ISO_OP_FDI:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				I.fp   = G.fp/H.fp;
				
				iso_vm_push(vm,I.uint);
				
				break;
			case ISO_OP_FPO:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				I.fp   = powf(G.fp,H.fp);
				
				iso_vm_push(vm,I.uint);
				
				break;
			case ISO_OP_FMO:
				H.uint = iso_vm_pop(vm);
				G.uint = iso_vm_pop(vm);
				I.fp   = fmod(G.fp,H.fp);
				
				iso_vm_push(vm,I.uint);
				
				break;
			default:
				if (OP<=0x0F) {
					D = iso_vm_fetch(vm);
					
					for (; D>0; D--) {
						A = 0;
						
						for (E=0; E<=OP; E++) {
							B = (iso_uint)iso_vm_fetch(vm);
							A = (A<<8)|B;
						}
						
						iso_vm_push(vm,A);
					}
					
					break;
				}
				
				iso_vm_interrupt(
					vm,
					ISO_INT_ILLEGAL_OPERATION
				);
		}
	} while (*INT==ISO_INT_NONE && !step);
}