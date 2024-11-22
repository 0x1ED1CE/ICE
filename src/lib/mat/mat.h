/*
MIT License

Copyright (c) 2024 Dice

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

#ifndef MAT_H
#define MAT_H

#define MAT_VERSION 8

#define MAT_ATTRIBUTE_META 0x00
#define MAT_ATTRIBUTE_MESH 0x10
#define MAT_ATTRIBUTE_VERT 0x22
#define MAT_ATTRIBUTE_NORM 0x32
#define MAT_ATTRIBUTE_TINT 0x42
#define MAT_ATTRIBUTE_TEXT 0x51
#define MAT_ATTRIBUTE_SKIN 0x60
#define MAT_ATTRIBUTE_ANIM 0x70
#define MAT_ATTRIBUTE_POSE 0x88
#define MAT_ATTRIBUTE_SLOT 0x90
#define MAT_ATTRIBUTE_TIME 0xA0

// MESH

typedef struct {
	char         *name_data;
	float        *vert_data;
	float        *norm_data;
	float        *tint_data;
	float        *text_data;
	unsigned int *skin_data;
	unsigned int  name_size;
	unsigned int  vert_size;
	unsigned int  norm_size;
	unsigned int  tint_size;
	unsigned int  text_size;
	unsigned int  skin_size;
} mat_mesh;

mat_mesh* mat_mesh_load(
	char        *filename,
	unsigned int id
);

void mat_mesh_free(
	mat_mesh *mesh
);

void mat_mesh_normalize(
	mat_mesh *mesh
);

// ANIMATION

typedef struct {
	char         *name_data;
	float        *pose_data;
	float        *time_data;
	unsigned int *slot_data;
	unsigned int  name_size;
	unsigned int  pose_size;
	unsigned int  slot_size;
	unsigned int  time_size;
} mat_animation;

mat_animation* mat_animation_load(
	char        *filename,
	unsigned int id
);

void mat_animation_free(
	mat_animation *animation
);

void mat_animation_pose(
	mat_animation *animation,
	float          time,
	unsigned int   bone,
	float          pose[16]
);

#endif

/************************[IMPLEMENTATION BEGINS HERE]*************************/

#ifdef MAT_IMPLEMENTATION

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <malloc.h>

#ifndef MAT_MIN
#define MAT_MIN(A,B) ((A)<(B)?(A):(B));
#endif

#ifndef MAT_MAX
#define MAT_MAX(A,B) ((A)>(B)?(A):(B));
#endif

// DECODING FUNCTIONS

static unsigned int mat_file_decode_uint(
	FILE *mat_file
) {
	return (
		(unsigned int)fgetc(mat_file)<<24|
		(unsigned int)fgetc(mat_file)<<16|
		(unsigned int)fgetc(mat_file)<<8|
		(unsigned int)fgetc(mat_file)
	);
}

static float mat_file_decode_fixed(
	FILE    *mat_file,
	unsigned int integer,
	unsigned int fraction
) {
	if (integer==0 && fraction==0) {
		return (float)fgetc(mat_file);
	}

	unsigned long encoded = 0;
	double        decoded;

	for (unsigned int i=0; i<integer+fraction; i++) {
		encoded=(encoded<<8)|(unsigned long)fgetc(mat_file);
	}

	decoded = (double)encoded;
	decoded = decoded/(1<<(fraction*8));
	decoded = decoded-(1<<(integer*8))/2;

	return (float)decoded;
}

void mat_file_decode(
	FILE         *mat_file,
	unsigned int  id,
	unsigned int  attribute,
	unsigned int *size,
	void        **data
) {
	*data = NULL;
	*size = 0;

	if (mat_file==NULL) return;

	fseek(mat_file,0,SEEK_END);
	unsigned int mat_file_size=ftell(mat_file);
	fseek(mat_file,0,SEEK_SET);

	while (
		!ferror(mat_file) &&
		(unsigned int)ftell(mat_file)<mat_file_size
	) {
		unsigned int attribute_id     = mat_file_decode_uint(mat_file);
		unsigned int attribute_type   = fgetc(mat_file);
		unsigned int attribute_format = fgetc(mat_file);
		unsigned int attribute_count  = mat_file_decode_uint(mat_file);

		if (
			attribute_id==id &&
			attribute_type==attribute
		) {
			switch(attribute) {
				case MAT_ATTRIBUTE_META:
					*data=calloc(
						attribute_count,
						sizeof(unsigned char)
					);
					break;
				case MAT_ATTRIBUTE_MESH:
				case MAT_ATTRIBUTE_ANIM:
					*data=calloc(
						attribute_count+1,
						sizeof(char)
					);
					break;
				case MAT_ATTRIBUTE_SKIN:
				case MAT_ATTRIBUTE_SLOT:
					*data=calloc(
						attribute_count,
						sizeof(unsigned int)
					);
					break;
				default:
					*data=calloc(
						attribute_count,
						sizeof(float)
					);
			}

			if (*data==NULL) {
				return;
			}

			*size=attribute_count;

			for (unsigned int i=0; i<attribute_count; i++) {
				float value=mat_file_decode_fixed(
					mat_file,
					attribute_format>>4,
					attribute_format&0x0F
				);

				switch(attribute) {
					case MAT_ATTRIBUTE_META:
						((unsigned char*)(*data))[i]=(unsigned char)value;
						break;
					case MAT_ATTRIBUTE_MESH:
					case MAT_ATTRIBUTE_ANIM:
						((char*)(*data))[i]=(char)value;
						break;
					case MAT_ATTRIBUTE_SKIN:
					case MAT_ATTRIBUTE_SLOT:
						((unsigned int*)(*data))[i]=(unsigned int)value;
						break;
					default:
						((float*)(*data))[i]=value;
				}
			}

			return;
		}

		if (attribute_format==0) {
			fseek(
				mat_file,
				ftell(mat_file)+
				attribute_count,
				SEEK_SET
			);
		} else {
			fseek(
				mat_file,
				ftell(mat_file)+
				attribute_count*
				((attribute_format>>4)+(attribute_format&0x0F)),
				SEEK_SET
			);
		}
	}
}

// MESH FUNCTIONS

mat_mesh* mat_mesh_load(
	char        *filename,
	unsigned int id
) {
	FILE *mat_file=fopen(
		filename,
		"rb"
	);

	if (mat_file==NULL) return NULL;

	mat_mesh *mesh=malloc(sizeof(mat_mesh));

	if (mesh==NULL) {
		fclose(mat_file);

		return NULL;
	}

	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_MESH,
		&mesh->name_size,
		(void**)&mesh->name_data
	);
	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_VERT,
		&mesh->vert_size,
		(void**)&mesh->vert_data
	);
	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_NORM,
		&mesh->norm_size,
		(void**)&mesh->norm_data
	);
	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_TINT,
		&mesh->tint_size,
		(void**)&mesh->tint_data
	);
	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_TEXT,
		&mesh->text_size,
		(void**)&mesh->text_data
	);
	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_SKIN,
		&mesh->skin_size,
		(void**)&mesh->skin_data
	);

	fclose(mat_file);

	return mesh;
}

void mat_mesh_free(
	mat_mesh *mesh
) {
	if (mesh==NULL) return;

	if (mesh->name_data!=NULL) free(mesh->name_data);
	if (mesh->vert_data!=NULL) free(mesh->vert_data);
	if (mesh->norm_data!=NULL) free(mesh->norm_data);
	if (mesh->tint_data!=NULL) free(mesh->tint_data);
	if (mesh->text_data!=NULL) free(mesh->text_data);
	if (mesh->skin_data!=NULL) free(mesh->skin_data);

	free(mesh);
}

void mat_mesh_normalize(
	mat_mesh *mesh
) {
	if (
		mesh==NULL ||
		mesh->vert_data==NULL
	) return;

	float min_x = FLT_MAX;
	float min_y = FLT_MAX;
	float min_z = FLT_MAX;

	float max_x = -FLT_MAX;
	float max_y = -FLT_MAX;
	float max_z = -FLT_MAX;

	for (unsigned int i=0; i<mesh->vert_size; i+=3) {
		min_x = MAT_MIN(min_x,mesh->vert_data[i]);
		min_y = MAT_MIN(min_y,mesh->vert_data[i+1]);
		min_z = MAT_MIN(min_z,mesh->vert_data[i+2]);

		max_x = MAT_MAX(max_x,mesh->vert_data[i]);
		max_y = MAT_MAX(max_y,mesh->vert_data[i+1]);
		max_z = MAT_MAX(max_z,mesh->vert_data[i+2]);
	}

	float size_x = max_x-min_x;
	float size_y = max_y-min_y;
	float size_z = max_z-min_z;

	float center_x = (min_x+max_x)/2;
	float center_y = (min_y+max_y)/2;
	float center_z = (min_z+max_z)/2;

	for (unsigned int i=0; i<mesh->vert_size; i+=3) {
		mesh->vert_data[i]   = (mesh->vert_data[i]-center_x)/size_x;
		mesh->vert_data[i+1] = (mesh->vert_data[i+1]-center_y)/size_y;
		mesh->vert_data[i+2] = (mesh->vert_data[i+2]-center_z)/size_z;
	}
}

// ANIMATION FUNCTIONS

void mat_animation_pose_interpolate(
	float pose_a[7],
	float pose_b[7],
	float pose_c[7],
	float t1
) {
	float t0 = 1-t1;

	float ax = pose_a[0];
	float ay = pose_a[1];
	float az = pose_a[2];
	float a0 = pose_a[3];
	float a1 = pose_a[4];
	float a2 = pose_a[5];
	float a3 = pose_a[6];

	float bx = pose_b[0];
	float by = pose_b[1];
	float bz = pose_b[2];
	float b0 = pose_b[3];
	float b1 = pose_b[4];
	float b2 = pose_b[5];
	float b3 = pose_b[6];
	
	float dot = a0*b0+a1*b1+a2*b2+a3*b3;

	if (dot<0) {
		b0 = -b0;
		b1 = -b1;
		b2 = -b2;
		b3 = -b3;
	}

	float c0 = a0*t0+b0*t1;
	float c1 = a1*t0+b1*t1;
	float c2 = a2*t0+b2*t1;
	float c3 = a3*t0+b3*t1;
	float cm = sqrtf(c0*c0+c1*c1+c2*c2+c3*c3);

	pose_c[0] = ax*t0+bx*t1;
	pose_c[1] = ay*t0+by*t1;
	pose_c[2] = az*t0+bz*t1;
	pose_c[3] = c0/cm;
	pose_c[4] = c1/cm;
	pose_c[5] = c2/cm;
	pose_c[6] = c3/cm;
}

void mat_animation_pose_matrix(
	float pose_data[7],
	float transform[16]
) {
	float qx = pose_data[3];
	float qy = pose_data[4];
	float qz = pose_data[5];
	float qw = pose_data[6];

	float qxqx = qx*qx;
	float qyqy = qy*qy;
	float qzqz = qz*qz;
	float qxqy = qx*qy;
	float qxqz = qx*qz;
	float qyqz = qy*qz;
	float qxqw = qx*qw;
	float qyqw = qy*qw;
	float qzqw = qz*qw;

	transform[0]  = 1-2*qyqy-2*qzqz;
	transform[1]  = 2*(qxqy-qzqw);
	transform[2]  = 2*(qxqz+qyqw);
	transform[3]  = pose_data[0];
	transform[4]  = 2*(qxqy+qzqw);
	transform[5]  = 1-2*qxqx-2*qzqz;
	transform[6]  = 2*(qyqz-qxqw);
	transform[7]  = pose_data[1];
	transform[8]  = 2*(qxqz-qyqw);
	transform[9]  = 2*(qyqz+qxqw);
	transform[10] = 1-2*qxqx-2*qyqy;
	transform[11] = pose_data[2];
	transform[12] = 0;
	transform[13] = 0;
	transform[14] = 0;
	transform[15] = 1;
}

mat_animation* mat_animation_load(
	char        *filename,
	unsigned int id
) {
	FILE *mat_file=fopen(
		filename,
		"rb"
	);

	if (mat_file==NULL) return NULL;

	mat_animation *animation=malloc(sizeof(mat_animation));

	if (animation==NULL) {
		fclose(mat_file);

		return NULL;
	}

	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_ANIM,
		&animation->name_size,
		(void**)&animation->name_data
	);
	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_POSE,
		&animation->pose_size,
		(void**)&animation->pose_data
	);
	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_SLOT,
		&animation->slot_size,
		(void**)&animation->slot_data
	);
	mat_file_decode(
		mat_file,
		id,
		MAT_ATTRIBUTE_TIME,
		&animation->time_size,
		(void**)&animation->time_data
	);

	fclose(mat_file);

	return animation;
}

void mat_animation_free(
	mat_animation *animation
) {
	if (animation==NULL) return;

	if (animation->name_data!=NULL) free(animation->name_data);
	if (animation->pose_data!=NULL) free(animation->pose_data);
	if (animation->slot_data!=NULL) free(animation->slot_data);
	if (animation->time_data!=NULL) free(animation->time_data);

	free(animation);
}

void mat_animation_pose(
	mat_animation *animation,
	float          time,
	unsigned int   bone,
	float          pose[16]
) {
	if (animation==NULL) return;

	float        *pose_data = animation->pose_data;
	float        *time_data = animation->time_data;
	unsigned int *slot_data = animation->slot_data;
	unsigned int  pose_size = animation->pose_size;
	unsigned int  time_size = animation->time_size;
	unsigned int  slot_size = animation->slot_size;

	float duration = time_data[time_size-1];

	unsigned int frame = (unsigned int)(time/duration*(float)(slot_size-1));

	mat_animation_pose_matrix(
		&pose_data[slot_data[frame]*7+bone*7],
		pose
	);
}

#endif
