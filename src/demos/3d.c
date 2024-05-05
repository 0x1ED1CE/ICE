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
	//V1,T1, V2,T2, V3,T3
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

ice_uint projection_id;
ice_uint model_id;

lmath_real camera_rotation[3] = {0,0,0};
lmath_real camera_position[3] = {0,0,0};

ice_uint load_model(
	const ice_real *vertices,
	const ice_real *uvs,
	const ice_uint *faces,
	const ice_uint faces_size
) {
	ice_uint vertices_id = ice_video_array_new(
		ICE_ARRAY_TYPE_POSITION_3,
		(faces_size/6)*9
	);
	ice_uint uvs_id = ice_video_array_new(
		ICE_ARRAY_TYPE_TEXTURE_2,
		faces_size
	);
	ice_uint model_id = ice_video_array_new(
		ICE_ARRAY_TYPE_MODEL,
		2
	);
	
	for (
		ice_uint f_i=0;
		f_i<faces_size;
		f_i+=6
	) {
		ice_uint v_a_i = (f_i/6)*9;
		ice_uint t_a_i = f_i;
		
		ice_uint v_i = (faces[f_i]-1)*3;
		ice_video_array_set(vertices_id,v_a_i,vertices[v_i]);
		ice_video_array_set(vertices_id,v_a_i+1,vertices[v_i+1]);
		ice_video_array_set(vertices_id,v_a_i+2,vertices[v_i+2]);
		
		v_i = (faces[f_i+1]-1)*2;
		ice_video_array_set(uvs_id,t_a_i,uvs[v_i]);
		ice_video_array_set(uvs_id,t_a_i+1,uvs[v_i+1]);
		
		v_i = (faces[f_i+2]-1)*3;
		ice_video_array_set(vertices_id,v_a_i+3,vertices[v_i]);
		ice_video_array_set(vertices_id,v_a_i+4,vertices[v_i+1]);
		ice_video_array_set(vertices_id,v_a_i+5,vertices[v_i+2]);
		
		v_i = (faces[f_i+3]-1)*2;
		ice_video_array_set(uvs_id,t_a_i+2,uvs[v_i]);
		ice_video_array_set(uvs_id,t_a_i+3,uvs[v_i+1]);
		
		v_i = (faces[f_i+4]-1)*3;
		ice_video_array_set(vertices_id,v_a_i+6,vertices[v_i]);
		ice_video_array_set(vertices_id,v_a_i+7,vertices[v_i+1]);
		ice_video_array_set(vertices_id,v_a_i+8,vertices[v_i+2]);
		
		v_i = (faces[f_i+5]-1)*2;
		ice_video_array_set(uvs_id,t_a_i+4,uvs[v_i]);
		ice_video_array_set(uvs_id,t_a_i+5,uvs[v_i+1]);
	}
	
	ice_video_array_set(model_id,0,(ice_real)vertices_id);
	ice_video_array_set(model_id,1,(ice_real)uvs_id);
	
	return model_id;
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
	
	lmath_matrix_44_perspective(
		view,
		70.0f*M_PI/180.f,
		640.0f/480.0f,
		0.1f,
		1000.0f
	);
	
	lmath_matrix_44_position_set(
		camera,
		(lmath_real[3]){0,0,3}
	);
	
	projection_id = ice_video_array_new(
		ICE_ARRAY_TYPE_MATRIX_16,
		16
	);
	
	model_id = load_model(
		cube_vertices,
		cube_uvs,
		cube_faces,
		sizeof(cube_faces)/sizeof(ice_uint)
	);
}

void ice_deinit() {}

ice_uint ice_update(
	ice_real tick
) {
	lmath_matrix_44_copy(view,render);
	lmath_matrix_44_copy(camera,inverse_camera);
	lmath_matrix_44_inverse(inverse_camera);
	lmath_matrix_44_multiply(render,inverse_camera);
	lmath_matrix_44_multiply(render,model);
	
	for (ice_uint i=0; i<16; i++) {
		ice_video_array_set(projection_id,i,render[i%4][i/4]);
	}
	
	ice_video_model_draw(
		texture_1,
		projection_id,
		model_id
	);
	
	move_camera((lmath_real)tick);
	
	return ice_input_get(0,0x01)==1.0f;
}