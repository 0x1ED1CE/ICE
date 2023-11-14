#include "ice.h"
#include "lib/iso/iso_vm.h"

#ifndef ISO_ICE_VIDEO_H
#define ISO_ICE_VIDEO_H

#define ISO_INT_VIDEO_BUFFER                 0x20
#define ISO_INT_VIDEO_FLUSH                  0x21
#define ISO_INT_VIDEO_TEXTURE_NEW            0x22
#define ISO_INT_VIDEO_TEXTURE_LOAD           0x23
#define ISO_INT_VIDEO_TEXTURE_DELETE         0x24
#define ISO_INT_VIDEO_TEXTURE_WIDTH_GET      0x25
#define ISO_INT_VIDEO_TEXTURE_HEIGHT_GET     0x26
#define ISO_INT_VIDEO_TEXTURE_CLEAR          0x27
#define ISO_INT_VIDEO_TEXTURE_PIXEL_DRAW     0x28
#define ISO_INT_VIDEO_TEXTURE_RECTANGLE_DRAW 0x29
#define ISO_INT_VIDEO_TEXTURE_TRIANGLE_DRAW  0x2A

void iso_ice_video_run(
	iso_vm *vm
) {
	if (vm->INT==ISO_INT_NONE)
		return;
	
	iso_uint A,B,C,D;
	
	switch(vm->INT) {
		case ISO_INT_VIDEO_BUFFER:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			ice_video_buffer();
			
			break;
		case ISO_INT_VIDEO_FLUSH:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			ice_video_flush();
			
			break;
		case ISO_INT_VIDEO_TEXTURE_NEW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			C = iso_vm_pop(vm);
			B = iso_vm_pop(vm);
			A = iso_vm_pop(vm);
			D = (iso_uint)ice_video_texture_new(
				(ice_uint)A,
				(ice_uint)B,
				(ice_char)C
			);
			
			iso_vm_push(vm,D);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_LOAD:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = (iso_uint)ice_video_texture_load(
				(ice_uint)A
			);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_DELETE:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			
			ice_video_texture_delete(
				(ice_uint)A
			);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_WIDTH_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = (iso_uint)ice_video_texture_width_get(
				(ice_uint)A
			);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_HEIGHT_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = (iso_uint)ice_video_texture_height_get(
				(ice_uint)A
			);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_CLEAR:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = vm->SP;
			
			ice_video_texture_clear(
				(ice_uint)A
			);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_PIXEL_DRAW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = vm->SP;
			
			ice_video_texture_pixel_draw(
				(ice_uint)vm->stack[A-7],
				*(ice_float *)&vm->stack[A-6],
				*(ice_float *)&vm->stack[A-5],
				*(ice_float *)&vm->stack[A-4],
				*(ice_float *)&vm->stack[A-3],
				*(ice_float *)&vm->stack[A-2],
				*(ice_float *)&vm->stack[A-1]
			);
			
			iso_vm_hop(vm,A-7);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_RECTANGLE_DRAW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = vm->SP;
			
			ice_video_texture_rectangle_draw(
				(ice_uint)vm->stack[A-14],
				(ice_uint)vm->stack[A-13],
				*(ice_float *)&vm->stack[A-12],
				*(ice_float *)&vm->stack[A-11],
				*(ice_float *)&vm->stack[A-10],
				*(ice_float *)&vm->stack[A-9],
				*(ice_float *)&vm->stack[A-8],
				*(ice_float *)&vm->stack[A-7],
				*(ice_float *)&vm->stack[A-6],
				*(ice_float *)&vm->stack[A-5],
				*(ice_float *)&vm->stack[A-4],
				*(ice_float *)&vm->stack[A-3],
				*(ice_float *)&vm->stack[A-2],
				*(ice_float *)&vm->stack[A-1]
			);
			
			iso_vm_hop(vm,A-14);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_TRIANGLE_DRAW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = vm->SP;
			
			ice_video_texture_triangle_draw(
				(ice_uint)vm->stack[A-29],
				(ice_uint)vm->stack[A-28],
				*(ice_float *)&vm->stack[A-27],
				*(ice_float *)&vm->stack[A-26],
				*(ice_float *)&vm->stack[A-25],
				*(ice_float *)&vm->stack[A-24],
				*(ice_float *)&vm->stack[A-23],
				*(ice_float *)&vm->stack[A-22],
				*(ice_float *)&vm->stack[A-21],
				*(ice_float *)&vm->stack[A-20],
				*(ice_float *)&vm->stack[A-19],
				*(ice_float *)&vm->stack[A-18],
				*(ice_float *)&vm->stack[A-17],
				*(ice_float *)&vm->stack[A-16],
				*(ice_float *)&vm->stack[A-15],
				*(ice_float *)&vm->stack[A-14],
				*(ice_float *)&vm->stack[A-13],
				*(ice_float *)&vm->stack[A-12],
				*(ice_float *)&vm->stack[A-11],
				*(ice_float *)&vm->stack[A-10],
				*(ice_float *)&vm->stack[A-9],
				*(ice_float *)&vm->stack[A-8],
				*(ice_float *)&vm->stack[A-7],
				*(ice_float *)&vm->stack[A-6],
				*(ice_float *)&vm->stack[A-5],
				*(ice_float *)&vm->stack[A-4],
				*(ice_float *)&vm->stack[A-3],
				*(ice_float *)&vm->stack[A-2],
				*(ice_float *)&vm->stack[A-1]
			);
			
			iso_vm_hop(vm,A-29);
			
			break;
	}
}

#endif