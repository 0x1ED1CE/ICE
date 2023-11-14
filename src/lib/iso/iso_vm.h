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

#include "iso.h"

#ifndef ISO_VM_H
#define ISO_VM_H

#define ISO_OP_INT 0x10 //INTERRUPT
#define ISO_OP_JMP 0x20 //JUMP
#define ISO_OP_JMC 0x21 //CONDITIONAL JUMP
#define ISO_OP_CEQ 0x22 //COMPARE IF EQUAL
#define ISO_OP_CNE 0x23 //COMPARE IF NOT EQUAL
#define ISO_OP_CLS 0x24 //COMPARE IF LESS THAN
#define ISO_OP_CLE 0x25 //COMPARE IF LESS OR EQUAL
#define ISO_OP_HOP 0x30 //SET STACK POSITION
#define ISO_OP_POS 0x31 //GET STACK POSITION
#define ISO_OP_DUP 0x32 //DUP
#define ISO_OP_POP 0x33 //POP
#define ISO_OP_SET 0x34 //SET ADDRESS
#define ISO_OP_GET 0x35 //GET ADDRESS
#define ISO_OP_ADD 0x40 //ADD
#define ISO_OP_SUB 0x41 //SUBTRACT
#define ISO_OP_MUL 0x42 //MULTIPLY
#define ISO_OP_DIV 0x43 //DIVIDE
#define ISO_OP_POW 0x44 //POWER
#define ISO_OP_MOD 0x45 //MODULO
#define ISO_OP_NOT 0x50 //NOT
#define ISO_OP_AND 0x51 //AND
#define ISO_OP_BOR 0x52 //OR
#define ISO_OP_XOR 0x53 //XOR
#define ISO_OP_LSH 0x54 //LEFT SHIFT
#define ISO_OP_RSH 0x55 //RIGHT SHIFT
#define ISO_OP_FTU 0x60 //FLOAT TO UNSIGNED
#define ISO_OP_UTF 0x61 //UNSIGNED TO FLOAT
#define ISO_OP_FEQ 0x62 //COMPARE IF FLOAT EQUAL
#define ISO_OP_FNE 0x63 //COMPARE IF FLOAT NOT EQUAL
#define ISO_OP_FLS 0x64 //COMPARE IF FLOAT LESS THAN
#define ISO_OP_FLE 0x65 //COMPARE IF FLOAT LESS OR EQUAL
#define ISO_OP_FAD 0x70 //FLOAT ADD
#define ISO_OP_FSU 0x71 //FLOAT SUBTRACT
#define ISO_OP_FMU 0x72 //FLOAT MULTIPLY
#define ISO_OP_FDI 0x73 //FLOAT DIVIDE
#define ISO_OP_FPO 0x74 //FLOAT POWER
#define ISO_OP_FMO 0x75 //FLOAT MODULO

#define ISO_INT_NONE              0x00
#define ISO_INT_END_OF_PROGRAM    0x01
#define ISO_INT_ILLEGAL_OPERATION 0x02
#define ISO_INT_INVALID_JUMP      0x03
#define ISO_INT_OUT_OF_BOUNDS     0x04
#define ISO_INT_STACK_OVERFLOW    0x05
#define ISO_INT_STACK_UNDERFLOW   0x06

typedef struct iso_vm iso_vm;

struct iso_vm {
	iso_uint  INT;
	iso_uint  PC;
	iso_uint  SP;
	iso_uint  program_size;
	iso_uint  stack_size;
	iso_char *program;
	iso_uint *stack;
};

void iso_vm_interrupt(
	iso_vm *vm,
	iso_uint interrupt
);

iso_char iso_vm_fetch(
	iso_vm *vm
);

void iso_vm_push(
	iso_vm *vm,
	iso_uint value
);

iso_uint iso_vm_pop(
	iso_vm *vm
);

void iso_vm_hop(
	iso_vm *vm,
	iso_uint address
);

void iso_vm_set(
	iso_vm *vm,
	iso_uint address,
	iso_uint value
);

iso_uint iso_vm_get(
	iso_vm *vm,
	iso_uint address
);

void iso_vm_jump(
	iso_vm *vm,
	iso_uint address
);

void iso_vm_run(
	iso_vm *vm,
	iso_uint step
);

#endif