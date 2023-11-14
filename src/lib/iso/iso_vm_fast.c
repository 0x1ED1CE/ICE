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
	iso_uint  PC      = vm->PC;
	iso_char *program = vm->program;
	
	vm->PC = PC+1;
	
	return program[PC];
}

void iso_vm_push(
	iso_vm *vm,
	iso_uint value
) {
	iso_uint  SP    = vm->SP;
	iso_uint *stack = vm->stack;
	
	vm->SP    = SP+1;
	stack[SP] = value;
}

iso_uint iso_vm_pop(
	iso_vm *vm
) {
	iso_uint  SP    = vm->SP;
	iso_uint *stack = vm->stack;
	
	vm->SP=SP-1;
	
	return stack[vm->SP];
}

void iso_vm_hop(
	iso_vm *vm,
	iso_uint address
) {
	vm->SP = address;
}

void iso_vm_set(
	iso_vm *vm,
	iso_uint address,
	iso_uint value
) {
	iso_uint *stack = vm->stack;
	
	stack[address]=value;
}

iso_uint iso_vm_get(
	iso_vm *vm,
	iso_uint address
) {
	iso_uint *stack = vm->stack;
	
	return stack[address];
}

void iso_vm_jump(
	iso_vm *vm,
	iso_uint address
) {
	vm->PC=address;
}

void iso_vm_run(
	iso_vm *vm,
	iso_uint step
) {
	if (vm->INT)
		return;
	
	iso_uint INT = vm->INT;
	iso_uint PC  = vm->PC;
	iso_uint SP  = vm->SP;
	
	iso_uint *stack   = vm->stack;
	iso_char *program = vm->program;
	
	iso_uint A,B,C;
	iso_char D,E;
	
	iso_word G = {0};
	iso_word H = {0};
	iso_word I = {0};
	
	iso_char OP;
	
	do {
		switch(OP=program[PC++]) {
			case ISO_OP_INT:
				INT = stack[--SP];
				 
				break;
			case ISO_OP_JMP:
				PC = stack[--SP];
				
				break;
			case ISO_OP_JMC:
				B = stack[--SP];
				A = stack[--SP];
				
				if (A)
					PC = B;
				
				break;
			case ISO_OP_CEQ:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A==B;
				
				break;
			case ISO_OP_CNE:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A!=B;
				
				break;
			case ISO_OP_CLS:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A<B;
				
				break;
			case ISO_OP_CLE:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A<=B;
				
				break;
			case ISO_OP_HOP:
				SP = stack[SP-1];
				
				break;
			case ISO_OP_POS:
				A = SP;
				
				stack[SP++]=A;
				
				break;
			case ISO_OP_DUP:
				A = stack[SP-1];
				
				stack[SP++]=A;
				
				break;
			case ISO_OP_POP:
				SP-=1;
				
				break;
			case ISO_OP_SET:
				A = stack[--SP];
				B = stack[--SP];
				
				stack[A] = B;
				
				break;
			case ISO_OP_GET:
				A = stack[--SP];
				
				stack[SP++] = stack[A];
				
				break;
			case ISO_OP_ADD:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A+B;
				
				break;
			case ISO_OP_SUB:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A-B;
				
				break;
			case ISO_OP_MUL:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A*B;
				
				break;
			case ISO_OP_DIV:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A/B;
				
				break;
			case ISO_OP_POW:
				B = stack[--SP];
				A = stack[--SP];
				C = 1;
				
				for (; B>0; B--) {
					C=C*A;
				}
				
				stack[SP++] = C;
				
				break;
			case ISO_OP_MOD:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A%B;
				
				break;
			case ISO_OP_NOT:
				stack[SP-1]=~stack[SP-1];
				
				break;
			case ISO_OP_AND:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A&B;
				
				break;
			case ISO_OP_BOR:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A|B;
				
				break;
			case ISO_OP_XOR:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A^B;
				
				break;
			case ISO_OP_LSH:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A<<B;
				
				break;
			case ISO_OP_RSH:
				B = stack[--SP];
				A = stack[--SP];
				
				stack[SP++] = A>>B;
				
				break;
			case ISO_OP_FTU:
				G.uint = stack[--SP];
				H.uint = (iso_uint)G.fp;
				
				stack[SP++] = H.uint;
				
				break;
			case ISO_OP_UTF:
				G.uint = stack[--SP];
				H.fp   = (iso_float)G.uint;
				
				stack[SP++] = H.uint;
				
				break;
			case ISO_OP_FEQ:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				
				stack[SP++] = G.fp==H.fp;
				
				break;
			case ISO_OP_FNE:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				
				stack[SP++] = G.fp!=H.fp;
				
				break;
			case ISO_OP_FLS:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				
				stack[SP++] = G.fp<H.fp;
				
				break;
			case ISO_OP_FLE:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				
				stack[SP++] = G.fp<=H.fp;
				
				break;
			case ISO_OP_FAD:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				I.fp   = G.fp+H.fp;
				
				stack[SP++] = I.uint;
				
				break;
			case ISO_OP_FSU:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				I.fp   = G.fp-H.fp;
				
				stack[SP++] = I.uint;
				
				break;
			case ISO_OP_FMU:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				I.fp   = G.fp*H.fp;
				
				stack[SP++] = I.uint;
				
				break;
			case ISO_OP_FDI:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				I.fp   = G.fp/H.fp;
				
				stack[SP++] = I.uint;
				
				break;
			case ISO_OP_FPO:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				I.fp   = powf(G.fp,H.fp);
				
				stack[SP++] = I.uint;
				
				break;
			case ISO_OP_FMO:
				H.uint = stack[--SP];
				G.uint = stack[--SP];
				I.fp   = fmod(G.fp,H.fp);
				
				stack[SP++] = I.uint;
				
				break;
			default:
				if (OP<=15) {
					D = program[PC++];
					
					for (; D>0; D--) {
						A = 0;
						
						for (E=0; E<OP; E++) {
							B = (iso_uint)program[PC++];
							A = (A<<8)|B;
						}
						
						stack[SP++] = A;
					}
					
					break;
				}
				
				INT=ISO_INT_ILLEGAL_OPERATION;
		}
	} while (INT==ISO_INT_NONE && !step);
	
	vm->INT = INT;
	vm->PC  = PC;
	vm->SP  = SP;
}