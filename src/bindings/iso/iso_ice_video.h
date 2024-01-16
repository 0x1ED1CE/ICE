#include "ice.h"
#include "lib/iso/iso_vm.h"

#ifndef ISO_ICE_VIDEO_H
#define ISO_ICE_VIDEO_H

#define ISO_INT_VIDEO_BUFFER                 0x2000
#define ISO_INT_VIDEO_TEXTURE_FLUSH          0x2001
#define ISO_INT_VIDEO_TEXTURE_NEW            0x2002
#define ISO_INT_VIDEO_TEXTURE_LOAD           0x2003
#define ISO_INT_VIDEO_TEXTURE_DELETE         0x2004
#define ISO_INT_VIDEO_TEXTURE_WIDTH_GET      0x2005
#define ISO_INT_VIDEO_TEXTURE_HEIGHT_GET     0x2006
#define ISO_INT_VIDEO_TEXTURE_CLEAR          0x2007
#define ISO_INT_VIDEO_TEXTURE_PIXEL_DRAW     0x2008
#define ISO_INT_VIDEO_TEXTURE_RECTANGLE_DRAW 0x2009
#define ISO_INT_VIDEO_TEXTURE_TRIANGLE_DRAW  0x200A
#define ISO_INT_VIDEO_VERTEX_FLUSH           0x2010
#define ISO_INT_VIDEO_VERTEX_NEW             0x2011
#define ISO_INT_VIDEO_VERTEX_DELETE          0x2012
#define ISO_INT_VIDEO_VERTEX_SIZE_GET        0x2013
#define ISO_INT_VIDEO_VERTEX_SET_UINT        0x2014
#define ISO_INT_VIDEO_VERTEX_GET_UINT        0x2015
#define ISO_INT_VIDEO_VERTEX_SET_SINT        0x2016
#define ISO_INT_VIDEO_VERTEX_GET_SINT        0x2017
#define ISO_INT_VIDEO_VERTEX_SET_REAL        0x2018
#define ISO_INT_VIDEO_VERTEX_GET_REAL        0x2019
#define ISO_INT_VIDEO_VERTEX_TEXTURE_DRAW    0x201A

void iso_ice_video_run(
	iso_vm *vm
) {
	if (vm->INT==ISO_INT_NONE)
		return;
	
	iso_uint A,B,C,D;
	volatile iso_word E;
	
	switch(vm->INT) {
		case ISO_INT_VIDEO_BUFFER:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			ice_video_buffer();
			
			break;
		case ISO_INT_VIDEO_TEXTURE_FLUSH:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			ice_video_texture_flush();
			
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
				(ice_sint)(*(ice_real *)&vm->stack[A-6]),
				(ice_sint)(*(ice_real *)&vm->stack[A-5]),
				(ice_char)vm->stack[A-4],
				(ice_char)vm->stack[A-3],
				(ice_char)vm->stack[A-2],
				(ice_char)vm->stack[A-1]
			);
			
			iso_vm_hop(vm,A-7);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_RECTANGLE_DRAW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = vm->SP;
			
			ice_video_texture_rectangle_draw(
				(ice_uint)vm->stack[A-14],
				(ice_uint)vm->stack[A-13],
				(ice_sint)(*(ice_real *)&vm->stack[A-12]),
				(ice_sint)(*(ice_real *)&vm->stack[A-11]),
				(ice_sint)(*(ice_real *)&vm->stack[A-10]),
				(ice_sint)(*(ice_real *)&vm->stack[A-9]),
				(ice_sint)(*(ice_real *)&vm->stack[A-8]),
				(ice_sint)(*(ice_real *)&vm->stack[A-7]),
				(ice_sint)(*(ice_real *)&vm->stack[A-6]),
				(ice_sint)(*(ice_real *)&vm->stack[A-5]),
				(ice_char)vm->stack[A-4],
				(ice_char)vm->stack[A-3],
				(ice_char)vm->stack[A-2],
				(ice_char)vm->stack[A-1]
			);
			
			iso_vm_hop(vm,A-14);
			
			break;
		case ISO_INT_VIDEO_TEXTURE_TRIANGLE_DRAW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = vm->SP;
			
			ice_video_texture_triangle_draw(
				(ice_uint)vm->stack[A-29],
				(ice_uint)vm->stack[A-28],
				(ice_sint)(*(ice_real *)&vm->stack[A-27]),
				(ice_sint)(*(ice_real *)&vm->stack[A-26]),
				(ice_sint)(*(ice_real *)&vm->stack[A-25]),
				(ice_sint)(*(ice_real *)&vm->stack[A-24]),
				(ice_sint)(*(ice_real *)&vm->stack[A-23]),
				(ice_sint)(*(ice_real *)&vm->stack[A-22]),
				(ice_sint)(*(ice_real *)&vm->stack[A-21]),
				(ice_sint)(*(ice_real *)&vm->stack[A-20]),
				(ice_sint)(*(ice_real *)&vm->stack[A-19]),
				(ice_sint)(*(ice_real *)&vm->stack[A-18]),
				(ice_sint)(*(ice_real *)&vm->stack[A-17]),
				(ice_sint)(*(ice_real *)&vm->stack[A-16]),
				(ice_sint)(*(ice_real *)&vm->stack[A-15]),
				(ice_sint)(*(ice_real *)&vm->stack[A-14]),
				(ice_sint)(*(ice_real *)&vm->stack[A-13]),
				(ice_uint)vm->stack[A-12],
				(ice_uint)vm->stack[A-11],
				(ice_uint)vm->stack[A-10],
				(ice_char)vm->stack[A-9],
				(ice_char)vm->stack[A-8],
				(ice_char)vm->stack[A-7],
				(ice_char)vm->stack[A-6],
				(ice_char)vm->stack[A-5],
				(ice_char)vm->stack[A-4],
				(ice_char)vm->stack[A-3],
				(ice_char)vm->stack[A-2],
				(ice_char)vm->stack[A-1]
			);
			
			iso_vm_hop(vm,A-29);
			
			break;
		case ISO_INT_VIDEO_VERTEX_FLUSH:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			ice_video_vertex_flush();
			
			break;
		case ISO_INT_VIDEO_VERTEX_NEW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = (iso_uint)ice_video_vertex_new(
				(ice_uint)A
			);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_VIDEO_VERTEX_DELETE:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			
			ice_video_vertex_delete(
				(ice_uint)A
			);
			
			break;
		case ISO_INT_VIDEO_VERTEX_SIZE_GET:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = (iso_uint)ice_video_vertex_size_get(
				(ice_uint)A
			);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_VIDEO_VERTEX_GET_UINT:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = iso_vm_pop(vm);
			C = (iso_uint)ice_video_vertex_get_uint(
				(ice_uint)A,
				(ice_uint)B
			);
			
			iso_vm_push(vm,B);
			
			break;
		case ISO_INT_VIDEO_VERTEX_SET_UINT:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = iso_vm_pop(vm);
			B = iso_vm_pop(vm);
			C = iso_vm_pop(vm);
			
			ice_video_vertex_set_uint(
				(ice_uint)A,
				(ice_uint)B,
				(ice_uint)C
			);
			
			break;
		case ISO_INT_VIDEO_VERTEX_GET_SINT:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A      = iso_vm_pop(vm);
			B      = iso_vm_pop(vm);
			E.real = (iso_real)ice_video_vertex_get_sint(
				(ice_uint)A,
				(ice_uint)B
			);
			
			iso_vm_push(vm,E.uint);
			
			break;
		case ISO_INT_VIDEO_VERTEX_SET_SINT:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A      = iso_vm_pop(vm);
			B      = iso_vm_pop(vm);
			E.uint = iso_vm_pop(vm);
			
			ice_video_vertex_set_sint(
				(ice_uint)A,
				(ice_uint)B,
				(ice_sint)E.real
			);
			
			break;
		case ISO_INT_VIDEO_VERTEX_GET_REAL:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A      = iso_vm_pop(vm);
			B      = iso_vm_pop(vm);
			E.real = (iso_real)ice_video_vertex_get_real(
				(ice_uint)A,
				(ice_uint)B
			);
			
			iso_vm_push(vm,E.uint);
			
			break;
		case ISO_INT_VIDEO_VERTEX_SET_REAL:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A      = iso_vm_pop(vm);
			B      = iso_vm_pop(vm);
			E.uint = iso_vm_pop(vm);
			
			ice_video_vertex_set_real(
				(ice_uint)A,
				(ice_uint)B,
				(ice_real)E.real
			);
			
			break;
		case ISO_INT_VIDEO_VERTEX_TEXTURE_DRAW:
			iso_vm_interrupt(vm,ISO_INT_NONE);
			
			A = vm->SP;
			
			ice_video_vertex_texture_draw(
				(ice_uint)vm->stack[A-21],
				(ice_uint)vm->stack[A-20],
				(ice_real [4][4]){
					{
						*(ice_real *)&vm->stack[A-19],
						*(ice_real *)&vm->stack[A-18],
						*(ice_real *)&vm->stack[A-17],
						*(ice_real *)&vm->stack[A-16]
					},
					{
						*(ice_real *)&vm->stack[A-15],
						*(ice_real *)&vm->stack[A-14],
						*(ice_real *)&vm->stack[A-13],
						*(ice_real *)&vm->stack[A-12]
					},
					{
						*(ice_real *)&vm->stack[A-11],
						*(ice_real *)&vm->stack[A-10],
						*(ice_real *)&vm->stack[A-9],
						*(ice_real *)&vm->stack[A-8]
					},
					{
						*(ice_real *)&vm->stack[A-7],
						*(ice_real *)&vm->stack[A-6],
						*(ice_real *)&vm->stack[A-5],
						*(ice_real *)&vm->stack[A-4]
					}
				},
				(ice_char)vm->stack[A-3],
				(ice_char)vm->stack[A-2],
				(ice_char)vm->stack[A-1]
			);
			
			iso_vm_hop(vm,A-21);
			
			break;
	}
}

#endif