#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "ice.h"

#include "lib/lmath/matrix.h"

const ice_real cube_vertices[]={
	-1.000000,  1.000000,  1.000000,
	-1.000000, -1.000000, -1.000000,
	-1.000000, -1.000000,  1.000000,
	-1.000000,  1.000000, -1.000000,
	 1.000000, -1.000000, -1.000000,
	 1.000000,  1.000000, -1.000000,
	 1.000000, -1.000000,  1.000000,
	 1.000000,  1.000000,  1.000000
};

const ice_real cube_uvs[]={
	0.999342, 0.666199,
	0.749671, 0.333801,
	0.999342, 0.333801,
	0.749671, 0.666199,
	0.500000, 0.333801,
	0.500000, 0.666199,
	0.250329, 0.333801,
	0.250329, 0.666199,
	0.000658, 0.333801,
	0.250329, 0.001403,
	0.500000, 0.001403,
	0.500000, 0.998597,
	0.000658, 0.666199,
	0.250329, 0.998597
};

const ice_uint cube_faces[]={
	1,1,  2,2,  3,3,
	4,4,  5,5,  2,2,
	6,6,  7,7,  5,5,
	8,8,  3,9,  7,7,
	5,5,  3,10, 2,11,
	4,12, 8,8,  6,6,
	1,1,  4,4,  2,2,
	4,4,  6,6,  5,5,
	6,6,  8,8,  7,7,
	8,8,  1,13, 3,9,
	5,5,  7,7,  3,10,
	4,12, 1,14, 8,8
};

ice_uint texture_1;
ice_uint font;

lmath_real view[4][4]           = LMATH_MATRIX_44_IDENTITY_TEMPLATE;
lmath_real camera[4][4]         = LMATH_MATRIX_44_IDENTITY_TEMPLATE;
lmath_real inverse_camera[4][4] = LMATH_MATRIX_44_IDENTITY_TEMPLATE;
lmath_real model[4][4]          = LMATH_MATRIX_44_IDENTITY_TEMPLATE;
lmath_real render[4][4]         = LMATH_MATRIX_44_IDENTITY_TEMPLATE;

ice_uint vertices_id;
ice_uint uvs_id;
ice_uint faces_id;

ice_char fps_text[32];

lmath_real camera_rotation[3] = {0,0,0};
lmath_real camera_position[3] = {0,0,0};

void print_graphics(
	ice_uint d_texture_id,
	ice_uint font_texture_id,
	ice_sint font_texture_width,
	ice_sint font_texture_height,
	ice_sint x,
	ice_sint y,
	ice_char *text,
	ice_char c_ar,
	ice_char c_ag,
	ice_char c_ab,
	ice_char c_aa
) {
	if (text==NULL) {
		return;
	}
	
	ice_uint char_index = 0;
	ice_sint ch         = (ice_sint)text[char_index];
	
	ice_sint lookup_x;
	ice_sint lookup_y;
	
	while (ch!='\0') {
		ch-=32;
		
		lookup_x = (ch%16)*font_texture_width;
		lookup_y = (ch/16)*font_texture_height;
		
		ice_video_texture_rectangle_draw(
			d_texture_id,
			font_texture_id,
			x, y,
			x+font_texture_width-1,
			y+font_texture_height-1,
			lookup_x,
			lookup_y,
			lookup_x+font_texture_width-1,
			lookup_y+font_texture_height-1,
			c_ar, c_ag, c_ab, c_aa
		);
		
		char_index++;
		ch=(ice_sint)text[char_index];
		x+=font_texture_width;
	}
}

void move_camera(
	lmath_real tick
) {
	camera_position[0] = 0;
	camera_position[1] = 0;
	camera_position[2] = 0;
	
	lmath_real pos_speed = 8*tick;
	lmath_real rot_speed = 4*tick;
	
	camera_position[0] += pos_speed*(lmath_real)ice_input_get(0,32);
	camera_position[0] -= pos_speed*(lmath_real)ice_input_get(0,30);
	camera_position[1] += pos_speed*(lmath_real)ice_input_get(0,18);
	camera_position[1] -= pos_speed*(lmath_real)ice_input_get(0,16);
	camera_position[2] += pos_speed*(lmath_real)ice_input_get(0,31);
	camera_position[2] -= pos_speed*(lmath_real)ice_input_get(0,17);
	
	camera_rotation[0] += rot_speed*(lmath_real)ice_input_get(0,72);
	camera_rotation[0] -= rot_speed*(lmath_real)ice_input_get(0,80);
	camera_rotation[1] += rot_speed*(lmath_real)ice_input_get(0,75);
	camera_rotation[1] -= rot_speed*(lmath_real)ice_input_get(0,77);
	
	lmath_matrix_44_transform(
		camera,
		camera_position
	);
	lmath_matrix_44_euler_set(
		camera,
		(lmath_real[3]){0,camera_rotation[1],0}
	);
	lmath_matrix_44_rotate_euler(
		camera,
		(lmath_real[3]){camera_rotation[0],0,0}
	);
}

void ice_init() {
	camera_rotation[0] = 0;
	camera_rotation[1] = 0;
	camera_rotation[2] = 0;
	
	texture_1 = ice_video_texture_load(2);
	font      = ice_video_texture_load(1);
	
	lmath_matrix_44_perspective(
		view,
		70.0f*M_PI/180.f,
		320.0f/200.0f,
		0.1f,
		1000.0f
	);
	
	lmath_matrix_44_position_set(
		camera,
		(lmath_real[3]){0,0,3}
	);
	
	vertices_id = ice_video_vertex_new(
		sizeof(cube_vertices)/sizeof(ice_real)
	);
	uvs_id = ice_video_vertex_new(
		sizeof(cube_uvs)/sizeof(ice_real)
	);
	faces_id = ice_video_vertex_new(
		sizeof(cube_faces)/sizeof(ice_uint)
	);
	
	for (ice_uint i=0; i<sizeof(cube_vertices)/sizeof(ice_real); i++) {
		ice_video_vertex_set_real(
			vertices_id,
			i,
			cube_vertices[i]
		);
	}
	
	for (ice_uint i=0; i<sizeof(cube_uvs)/sizeof(ice_real); i+=2) {
		ice_video_vertex_set_real(
			uvs_id,
			i,
			cube_uvs[i]
		);
		ice_video_vertex_set_real(
			uvs_id,
			i+1,
			1-cube_uvs[i+1]
		);
	}
	
	for (ice_uint i=0; i<sizeof(cube_faces)/sizeof(ice_uint); i++) {
		ice_video_vertex_set_uint(
			faces_id,
			i,
			cube_faces[i]-1
		);
	}
}

void ice_deinit() {}

ice_uint ice_update(
	ice_real tick
) {
	ice_video_texture_clear(0);
		
	lmath_matrix_44_copy(view,render);
	lmath_matrix_44_copy(camera,inverse_camera);
	lmath_matrix_44_inverse(inverse_camera);
	lmath_matrix_44_multiply(render,inverse_camera);
	lmath_matrix_44_multiply(render,model);
	
	ice_video_vertex_texture_draw(
		0,
		texture_1,
		(ice_real (*)[4])render,
		vertices_id,
		uvs_id,
		faces_id
	);
	
	sprintf(
		(char *)fps_text,
		"FPS: %.0f",
		1/tick
	);
	
	print_graphics(
		0, font,
		12, 16,
		5, 5,
		fps_text,
		255, 255, 255, 255
	);
	
	move_camera((lmath_real)tick);
	
	return ice_input_get(0,0x01)==1.0f;
}