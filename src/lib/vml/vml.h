/*
Vector Matrix Library

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

#ifndef VML_H
#define VML_H

#define VML_VERSION_MAJOR 1
#define VML_VERSION_MINOR 1
#define VML_VERSION_PATCH 0

typedef float vml_real;

// SCALAR FUNCTIONS

#define VML_PI 3.1415926535898

static inline vml_real vml_min(
	vml_real a,
	vml_real b
);

static inline vml_real vml_max(
	vml_real a,
	vml_real b
);

static inline vml_real vml_floor(
	vml_real a
);

static inline vml_real vml_ceil(
	vml_real a
);

static inline vml_real vml_abs(
	vml_real a
);

static inline vml_real vml_sign(
	vml_real a
);

static inline vml_real vml_lerp(
	vml_real a,
	vml_real b,
	vml_real t
);

static inline vml_real vml_pow(
	vml_real a,
	vml_real b
);

static inline vml_real vml_mod(
	vml_real a,
	vml_real b
);

static inline vml_real vml_log(
	vml_real a
);

static inline vml_real vml_exp(
	vml_real a
);

static inline vml_real vml_deg(
	vml_real a
);

static inline vml_real vml_rad(
	vml_real a
);

static inline vml_real vml_sin(
	vml_real a
);

static inline vml_real vml_cos(
	vml_real a
);

static inline vml_real vml_tan(
	vml_real a
);

static inline vml_real vml_asin(
	vml_real a
);

static inline vml_real vml_acos(
	vml_real a
);

static inline vml_real vml_atan(
	vml_real a
);

static inline vml_real vml_atan2(
	vml_real x,
	vml_real y
);

// VEC2 FUNCTIONS

static inline void vml_vec2_num(
	vml_real a,
	vml_real b[2]
);

static inline void vml_vec2_neg(
	vml_real a[2],
	vml_real b[2]
);

static inline void vml_vec2_abs(
	vml_real a[2],
	vml_real b[2]
);

static inline void vml_vec2_mov(
	vml_real a[2],
	vml_real b[2]
);

static inline void vml_vec2_add(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
);

static inline void vml_vec2_sub(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
);

static inline void vml_vec2_mul(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
);

static inline void vml_vec2_div(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
);

static inline void vml_vec2_pow(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
);

static inline void vml_vec2_mod(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
);

static inline void vml_vec2_dot(
	vml_real  a[2],
	vml_real  b[2],
	vml_real *c
);

static inline void vml_vec2_mag(
	vml_real  a[2],
	vml_real *b
);

static inline void vml_vec2_unit(
	vml_real a[2],
	vml_real b[2]
);

static inline void vml_vec2_lerp(
	vml_real a[2],
	vml_real b[2],
	vml_real t,
	vml_real c[2]
);

// VEC3 FUNCTIONS

static inline void vml_vec3_num(
	vml_real a,
	vml_real b[3]
);

static inline void vml_vec3_neg(
	vml_real a[3],
	vml_real b[3]
);

static inline void vml_vec3_abs(
	vml_real a[3],
	vml_real b[3]
);

static inline void vml_vec3_mov(
	vml_real a[3],
	vml_real b[3]
);

static inline void vml_vec3_add(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
);

static inline void vml_vec3_sub(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
);

static inline void vml_vec3_mul(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
);

static inline void vml_vec3_div(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
);

static inline void vml_vec3_pow(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
);

static inline void vml_vec3_mod(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
);

static inline void vml_vec3_dot(
	vml_real  a[3],
	vml_real  b[3],
	vml_real *c
);

static inline void vml_vec3_cross(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
);

static inline void vml_vec3_mag(
	vml_real  a[3],
	vml_real *b
);

static inline void vml_vec3_unit(
	vml_real a[3],
	vml_real b[3]
);

static inline void vml_vec3_lerp(
	vml_real a[3],
	vml_real b[3],
	vml_real t,
	vml_real c[3]
);

// QUATERNION FUNCTIONS

void vml_quat_unit(
	vml_real a[4],
	vml_real b[4]
);

void vml_quat_lerp(
	vml_real a[4],
	vml_real b[4],
	vml_real t,
	vml_real c[4]
);

// MAT3 FUNCTIONS

static inline void vml_mat3_id(
	vml_real a[9]
);

static inline void vml_mat3_mov(
	vml_real a[9],
	vml_real b[9]
);

void vml_mat3_transpose(
	vml_real a[9],
	vml_real b[9]
);

void vml_mat3_vec3_mul(
	vml_real a[9],
	vml_real b[3],
	vml_real c[3]
);

void vml_mat3_mul(
	vml_real a[9],
	vml_real b[9],
	vml_real c[9]
);

void vml_mat3_inv(
	vml_real a[9],
	vml_real b[9]
);

void vml_mat3_scale_set(
	vml_real a[9],
	vml_real b[3],
	vml_real c[9]
);

// MAT4 FUNCTIONS

static inline void vml_mat4_id(
	vml_real a[16]
);

static inline void vml_mat4_mov(
	vml_real a[16],
	vml_real b[16]
);

void vml_mat4_transpose(
	vml_real a[16],
	vml_real b[16]
);

void vml_mat4_vec3_mul(
	vml_real a[16],
	vml_real b[3],
	vml_real c[3]
);

void vml_mat4_vec4_mul(
	vml_real a[16],
	vml_real b[4],
	vml_real c[4]
);

void vml_mat4_mul(
	vml_real a[16],
	vml_real b[16],
	vml_real c[16]
);

void vml_mat4_inv(
	vml_real a[16],
	vml_real b[16]
);

void vml_mat4_mat3_get(
	vml_real a[16],
	vml_real b[9]
);

void vml_mat4_euler_set(
	vml_real a[16],
	vml_real b[3],
	vml_real c[16]
);

void vml_mat4_euler_get(
	vml_real a[16],
	vml_real b[3]
);

void vml_mat4_quat_set(
	vml_real a[16],
	vml_real b[4],
	vml_real c[16]
);

void vml_mat4_quat_get(
	vml_real a[16],
	vml_real b[4]
);

void vml_mat4_axis_set(
	vml_real a[16],
	vml_real b[4],
	vml_real c[16]
);

void vml_mat4_axis_get(
	vml_real a[16],
	vml_real b[4]
);

void vml_mat4_look_set(
	vml_real a[16],
	vml_real b[3],
	vml_real c[3],
	vml_real d[16]
);

void vml_mat4_look_get(
	vml_real a[16],
	vml_real b[3]
);

void vml_mat4_pos_set(
	vml_real a[16],
	vml_real b[3],
	vml_real c[16]
);

void vml_mat4_pos_get(
	vml_real a[16],
	vml_real b[3]
);

void vml_mat4_scale_set(
	vml_real a[16],
	vml_real b[3],
	vml_real c[16]
);

void vml_mat4_scale_get(
	vml_real a[16],
	vml_real b[3]
);

void vml_mat4_lerp(
	vml_real a[16],
	vml_real b[16],
	vml_real t,
	vml_real c[16]
);

void vml_mat4_perspective(
	vml_real v, // Field of view
	vml_real r, // Aspect ratio
	vml_real n, // Near plane
	vml_real f, // Far plane
	vml_real a[16]
);

void vml_mat4_orthographic(
	vml_real l, // Left
	vml_real r, // Right
	vml_real t, // Top
	vml_real b, // Bottom
	vml_real n, // Near
	vml_real f, // Far
	vml_real a[16]
);

#endif

/************************[IMPLEMENTATION BEGINS HERE]*************************/

#ifdef VML_IMPLEMENTATION
#ifndef VML_C
#define VML_C

#include <math.h>

// SCALAR FUNCTIONS

static inline vml_real vml_min(
	vml_real a,
	vml_real b
) {
	return (a<b)?a:b;
}

static inline vml_real vml_max(
	vml_real a,
	vml_real b
) {
	return (a>b)?a:b;
}

static inline vml_real vml_floor(
	vml_real a
) {
	return floorf(a);
}

static inline vml_real vml_ceil(
	vml_real a
) {
	return ceilf(a);
}

static inline vml_real vml_abs(
	vml_real a
) {
	return (a<0)?-a:a;
}

static inline vml_real vml_sign(
	vml_real a
) {
	if (a==0) return 0;

	return (a<0)?-1:1;
}

static inline vml_real vml_lerp(
	vml_real a,
	vml_real b,
	vml_real t
) {
	return a*(1-t)+b*t;
}

static inline vml_real vml_pow(
	vml_real a,
	vml_real b
) {
	return powf(a,b);
}

static inline vml_real vml_mod(
	vml_real a,
	vml_real b
) {
	return fmodf(a,b);
}

static inline vml_real vml_log(
	vml_real a
) {
	return logf(a);
}

static inline vml_real vml_exp(
	vml_real a
) {
	return expf(a);
}

static inline vml_real vml_deg(
	vml_real a
) {
	return a*180.0f/VML_PI;
}

static inline vml_real vml_rad(
	vml_real a
) {
	return a*VML_PI/180.0f;
}

static inline vml_real vml_sin(
	vml_real a
) {
	return sinf(a);
}

static inline vml_real vml_cos(
	vml_real a
) {
	return cosf(a);
}

static inline vml_real vml_tan(
	vml_real a
) {
	return tanf(a);
}

static inline vml_real vml_asin(
	vml_real a
) {
	return asinf(a);
}

static inline vml_real vml_acos(
	vml_real a
) {
	return acosf(a);
}

static inline vml_real vml_atan(
	vml_real a
) {
	return atanf(a);
}

static inline vml_real vml_atan2(
	vml_real x,
	vml_real y
) {
	return atan2f(x,y);
}

// VEC2 FUNCTIONS

static inline void vml_vec2_num(
	vml_real a,
	vml_real b[2]
) {
	b[0] = a;
	b[1] = a;
}

static inline void vml_vec2_neg(
	vml_real a[2],
	vml_real b[2]
) {
	b[0] = -a[0];
	b[1] = -a[1];
}

static inline void vml_vec2_abs(
	vml_real a[2],
	vml_real b[2]
) {
	b[0] = vml_abs(a[0]);
	b[1] = vml_abs(a[1]);
}

static inline void vml_vec2_mov(
	vml_real a[2],
	vml_real b[2]
) {
	b[0] = a[0];
	b[1] = a[1];
}

static inline void vml_vec2_add(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
) {
	c[0] = a[0]+b[0];
	c[1] = a[1]+b[1];
}

static inline void vml_vec2_sub(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
) {
	c[0] = a[0]-b[0];
	c[1] = a[1]-b[1];
}

static inline void vml_vec2_mul(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
) {
	c[0] = a[0]*b[0];
	c[1] = a[1]*b[1];
}

static inline void vml_vec2_div(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
) {
	c[0] = a[0]/b[0];
	c[1] = a[1]/b[1];
}

static inline void vml_vec2_pow(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
) {
	c[0] = powf(a[0],b[0]);
	c[1] = powf(a[1],b[1]);
}

static inline void vml_vec2_mod(
	vml_real a[2],
	vml_real b[2],
	vml_real c[2]
) {
	c[0] = fmodf(a[0],b[0]);
	c[1] = fmodf(a[1],b[1]);
}

static inline void vml_vec2_dot(
	vml_real  a[2],
	vml_real  b[2],
	vml_real *c
) {
	*c = (a[0]*b[0])+(a[1]*b[1]);
}

static inline void vml_vec2_unit(
	vml_real a[2],
	vml_real b[2]
) {
	vml_real m=sqrtf(
		a[0]*a[0]+
		a[1]*a[1]
	);

	if (m==0) {
		b[0] = 0;
		b[1] = 0;

		return;
	}

	b[0] = a[0]/m;
	b[1] = a[1]/m;
}

static inline void vml_vec2_mag(
	vml_real  a[2],
	vml_real *b
) {
	*b=sqrtf(
		a[0]*a[0]+
		a[1]*a[1]
	);
}

static inline void vml_vec2_lerp(
	vml_real a[2],
	vml_real b[2],
	vml_real t,
	vml_real c[2]
) {
	vml_real it = 1-t;

	c[0] = vml_lerp(a[0],b[0],t);
	c[1] = vml_lerp(a[1],b[1],t);
}

// VEC3 FUNCTIONS

static inline void vml_vec3_num(
	vml_real a,
	vml_real b[3]
) {
	b[0] = a;
	b[1] = a;
	b[2] = a;
}

static inline void vml_vec3_neg(
	vml_real a[3],
	vml_real b[3]
) {
	b[0] = -a[0];
	b[1] = -a[1];
	b[2] = -a[2];
}

static inline void vml_vec3_abs(
	vml_real a[3],
	vml_real b[3]
) {
	b[0] = vml_abs(a[0]);
	b[1] = vml_abs(a[1]);
	b[2] = vml_abs(a[2]);
}

static inline void vml_vec3_mov(
	vml_real a[3],
	vml_real b[3]
) {
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
}

static inline void vml_vec3_add(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
) {
	c[0] = a[0]+b[0];
	c[1] = a[1]+b[1];
	c[2] = a[2]+b[2];
}

static inline void vml_vec3_sub(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
) {
	c[0] = a[0]-b[0];
	c[1] = a[1]-b[1];
	c[2] = a[2]-b[2];
}

static inline void vml_vec3_mul(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
) {
	c[0] = a[0]*b[0];
	c[1] = a[1]*b[1];
	c[2] = a[2]*b[2];
}

static inline void vml_vec3_div(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
) {
	c[0] = a[0]/b[0];
	c[1] = a[1]/b[1];
	c[2] = a[2]/b[2];
}

static inline void vml_vec3_dot(
	vml_real  a[3],
	vml_real  b[3],
	vml_real *c
) {
	*c = (a[0]*b[0])+(a[1]*b[1])+(a[2]*b[2]);
}

static inline void vml_vec3_cross(
	vml_real a[3],
	vml_real b[3],
	vml_real c[3]
) {
	vml_real r0 = a[1]*b[2]-a[2]*b[1];
	vml_real r1 = a[2]*b[0]-a[0]*b[2];
	vml_real r2 = a[0]*b[1]-a[1]*b[0];

	c[0] = r0;
	c[1] = r1;
	c[2] = r2;
}

static inline void vml_vec3_unit(
	vml_real a[3],
	vml_real b[3]
) {
	vml_real m=sqrtf(
		a[0]*a[0]+
		a[1]*a[1]+
		a[2]*a[2]
	);

	if (m==0) {
		b[0] = 0;
		b[1] = 0;
		b[2] = 0;

		return;
	}

	m = 1.0f/m;

	b[0] = a[0]*m;
	b[1] = a[1]*m;
	b[2] = a[2]*m;
}

static inline void vml_vec3_mag(
	vml_real  a[3],
	vml_real *b
) {
	*b=sqrtf(
		a[0]*a[0]+
		a[1]*a[1]+
		a[2]*a[2]
	);
}

static inline void vml_vec3_lerp(
	vml_real a[3],
	vml_real b[3],
	vml_real t,
	vml_real c[3]
) {
	vml_real it = 1-t;

	c[0] = vml_lerp(a[0],b[0],t);
	c[1] = vml_lerp(a[1],b[1],t);
	c[2] = vml_lerp(a[2],b[2],t);
}

// QUATERNION FUNCTIONS

void vml_quat_unit(
	vml_real a[4],
	vml_real b[4]
) {
	vml_real m=sqrtf(
		a[0]*a[0]+
		a[1]*a[1]+
		a[2]*a[2]+
		a[3]*a[3]
	);

	if (m==0) {
		b[0] = 0;
		b[1] = 0;
		b[2] = 0;
		b[3] = 0;

		return;
	}

	m = 1.0f/m;

	b[0] = a[0]*m;
	b[1] = a[1]*m;
	b[2] = a[2]*m;
	b[3] = a[3]*m;
}

void vml_quat_lerp(
	vml_real a[4],
	vml_real b[4],
	vml_real t,
	vml_real c[4]
) {
	vml_real ax = a[0];
	vml_real ay = a[1];
	vml_real az = a[2];
	vml_real aw = a[3];
	vml_real bx = b[0];
	vml_real by = b[1];
	vml_real bz = b[2];
	vml_real bw = b[3];

	vml_real am = 1.0f/sqrtf(ax*ax+ay*ay+az*az+aw*aw);
	vml_real bm = 1.0f/sqrtf(bx*bx+by*by+bz*bz+bw*bw);

	ax *= am;
	ay *= am;
	az *= am;
	aw *= am;
	bx *= bm;
	by *= bm;
	bz *= bm;
	bw *= bm;

	vml_real dot = ax*bx+ay*by+az*bz+aw*bw;

	if (dot<0) {
		bx  = -bx;
		by  = -by;
		bz  = -bz;
		bw  = -bw;
		dot = -dot;
	}

	if (dot>0.9995) {
		vml_real cx = vml_lerp(ax,bx,t);
		vml_real cy = vml_lerp(ay,by,t);
		vml_real cz = vml_lerp(az,bz,t);
		vml_real cw = vml_lerp(aw,bw,t);
		vml_real cm = 1.0f/sqrtf(cx*cx+cy*cy+cz*cz+cw*cw);

		c[0] = cx*cm;
		c[1] = cy*cm;
		c[2] = cz*cm;
		c[3] = cw*cm;

		return;
	}

	vml_real t0  = acosf(dot);
	vml_real t1  = t0*t;
	vml_real st0 = sin(t0);
	vml_real st1 = sin(t1);
	vml_real s0  = cosf(t1)-dot*st1/st0;
	vml_real s1  = st1/st0;

	c[0] = ax*s0+bx*s1;
	c[1] = ay*s0+by*s1;
	c[2] = az*s0+bz*s1;
	c[3] = aw*s0+bw*s1;
}

// MAT3 FUNCTIONS

static inline void vml_mat3_id(
	vml_real a[9]
) {
	a[0] = 1;
	a[1] = 0;
	a[2] = 0;
	a[3] = 0;
	a[4] = 1;
	a[5] = 0;
	a[6] = 0;
	a[7] = 0;
	a[8] = 1;
}

static inline void vml_mat3_mov(
	vml_real a[9],
	vml_real b[9]
) {
	for (unsigned int i=0; i<9; i++) {
		b[i] = a[i];
	}
}

void vml_mat3_transpose(
	vml_real a[9],
	vml_real b[9]
) {
	vml_real a00 = a[0];
	vml_real a01 = a[1];
	vml_real a02 = a[2];
	vml_real a10 = a[3];
	vml_real a11 = a[4];
	vml_real a12 = a[5];
	vml_real a20 = a[6];
	vml_real a21 = a[7];
	vml_real a22 = a[8];

	b[0] = a00;
	b[1] = a10;
	b[2] = a20;
	b[3] = a01;
	b[4] = a11;
	b[5] = a21;
	b[6] = a02;
	b[7] = a12;
	b[8] = a22;
}

void vml_mat3_vec3_mul(
	vml_real a[9],
	vml_real b[3],
	vml_real c[3]
) {
	vml_real r0 = b[0]*a[0]+b[1]*a[1]+b[2]*a[2];
	vml_real r1 = b[0]*a[3]+b[1]*a[4]+b[2]*a[5];
	vml_real r2 = b[0]*a[6]+b[1]*a[7]+b[2]*a[8];

	c[0] = r0;
	c[1] = r1;
	c[2] = r2;
}

void vml_mat3_mul(
	vml_real a[9],
	vml_real b[9],
	vml_real c[9]
) {
	vml_real a00 = a[0];
	vml_real a01 = a[1];
	vml_real a02 = a[2];
	vml_real a10 = a[3];
	vml_real a11 = a[4];
	vml_real a12 = a[5];
	vml_real a20 = a[6];
	vml_real a21 = a[7];
	vml_real a22 = a[8];

	vml_real b00 = b[0];
	vml_real b01 = b[1];
	vml_real b02 = b[2];
	vml_real b10 = b[3];
	vml_real b11 = b[4];
	vml_real b12 = b[5];
	vml_real b20 = b[6];
	vml_real b21 = b[7];
	vml_real b22 = b[8];

	c[0] = a00*b00+a01*b10+a02*b20;
	c[1] = a00*b01+a01*b11+a02*b21;
	c[2] = a00*b02+a01*b12+a02*b22;
	c[3] = a10*b00+a11*b10+a12*b20;
	c[4] = a10*b01+a11*b11+a12*b21;
	c[5] = a10*b02+a11*b12+a12*b22;
	c[6] = a20*b00+a21*b10+a22*b20;
	c[7] = a20*b01+a21*b11+a22*b21;
	c[8] = a20*b02+a21*b12+a22*b22;
}

void vml_mat3_inv(
	vml_real a[9],
	vml_real b[9]
) {
	vml_real a00 = a[0];
	vml_real a01 = a[1];
	vml_real a02 = a[2];
	vml_real a10 = a[3];
	vml_real a11 = a[4];
	vml_real a12 = a[5];
	vml_real a20 = a[6];
	vml_real a21 = a[7];
	vml_real a22 = a[8];

	vml_real det=1.0f/(
		a00*(a11*a22-a21*a12)-
		a01*(a10*a22-a12*a20)+
		a02*(a10*a21-a11*a20)
	);

	b[0] = (a11*a22-a21*a12)*det;
	b[1] = (a02*a21-a01*a22)*det;
	b[2] = (a01*a12-a02*a11)*det;
	b[3] = (a12*a20-a10*a22)*det;
	b[4] = (a00*a22-a02*a20)*det;
	b[5] = (a10*a02-a00*a12)*det;
	b[6] = (a10*a21-a20*a11)*det;
	b[7] = (a20*a01-a00*a21)*det;
	b[8] = (a00*a11-a10*a01)*det;
}

void vml_mat3_scale_set(
	vml_real a[9],
	vml_real b[3],
	vml_real c[9]
) {
	vml_mat3_mul(a,(vml_real[9]){
		b[0], 0,    0,
		0,    b[1], 0,
		0,    0,    b[2]
	},c);
}

// MAT4 FUNCTIONS

static inline void vml_mat4_id(
	vml_real a[16]
) {
	a[0]  = 1;
	a[1]  = 0;
	a[2]  = 0;
	a[3]  = 0;
	a[4]  = 0;
	a[5]  = 1;
	a[6]  = 0;
	a[7]  = 0;
	a[8]  = 0;
	a[9]  = 0;
	a[10] = 1;
	a[11] = 0;
	a[12] = 0;
	a[13] = 0;
	a[14] = 0;
	a[15] = 1;
}

static inline void vml_mat4_mov(
	vml_real a[16],
	vml_real b[16]
) {
	for (unsigned int i=0; i<16; i++) {
		b[i] = a[i];
	}
}


void vml_mat4_transpose(
	vml_real a[16],
	vml_real b[16]
) {
	vml_real a00 = a[0];
	vml_real a01 = a[1];
	vml_real a02 = a[2];
	vml_real a03 = a[3];
	vml_real a10 = a[4];
	vml_real a11 = a[5];
	vml_real a12 = a[6];
	vml_real a13 = a[7];
	vml_real a20 = a[8];
	vml_real a21 = a[9];
	vml_real a22 = a[10];
	vml_real a23 = a[11];
	vml_real a30 = a[12];
	vml_real a31 = a[13];
	vml_real a32 = a[14];
	vml_real a33 = a[15];

	b[0]  = a00;
	b[1]  = a10;
	b[2]  = a20;
	b[3]  = a30;
	b[4]  = a01;
	b[5]  = a11;
	b[6]  = a21;
	b[7]  = a31;
	b[8]  = a02;
	b[9]  = a12;
	b[10] = a22;
	b[11] = a32;
	b[12] = a03;
	b[13] = a13;
	b[14] = a23;
	b[15] = a33;
}

void vml_mat4_vec3_mul(
	vml_real a[16],
	vml_real b[3],
	vml_real c[3]
) {
	vml_real r0 = b[0]*a[0]+b[1]*a[1]+b[2]*a[2]+a[3];
	vml_real r1 = b[0]*a[4]+b[1]*a[5]+b[2]*a[6]+a[7];
	vml_real r2 = b[0]*a[8]+b[1]*a[9]+b[2]*a[10]+a[11];

	c[0] = r0;
	c[1] = r1;
	c[2] = r2;
}

void vml_mat4_mul(
	vml_real a[16],
	vml_real b[16],
	vml_real c[16]
) {
	vml_real a00 = a[0];
	vml_real a01 = a[1];
	vml_real a02 = a[2];
	vml_real a03 = a[3];
	vml_real a10 = a[4];
	vml_real a11 = a[5];
	vml_real a12 = a[6];
	vml_real a13 = a[7];
	vml_real a20 = a[8];
	vml_real a21 = a[9];
	vml_real a22 = a[10];
	vml_real a23 = a[11];
	vml_real a30 = a[12];
	vml_real a31 = a[13];
	vml_real a32 = a[14];
	vml_real a33 = a[15];

	vml_real b00 = b[0];
	vml_real b01 = b[1];
	vml_real b02 = b[2];
	vml_real b03 = b[3];
	vml_real b10 = b[4];
	vml_real b11 = b[5];
	vml_real b12 = b[6];
	vml_real b13 = b[7];
	vml_real b20 = b[8];
	vml_real b21 = b[9];
	vml_real b22 = b[10];
	vml_real b23 = b[11];
	vml_real b30 = b[12];
	vml_real b31 = b[13];
	vml_real b32 = b[14];
	vml_real b33 = b[15];

	c[0]  = a00*b00+a01*b10+a02*b20+a03*b30;
	c[1]  = a00*b01+a01*b11+a02*b21+a03*b31;
	c[2]  = a00*b02+a01*b12+a02*b22+a03*b32;
	c[3]  = a00*b03+a01*b13+a02*b23+a03*b33;
	c[4]  = a10*b00+a11*b10+a12*b20+a13*b30;
	c[5]  = a10*b01+a11*b11+a12*b21+a13*b31;
	c[6]  = a10*b02+a11*b12+a12*b22+a13*b32;
	c[7]  = a10*b03+a11*b13+a12*b23+a13*b33;
	c[8]  = a20*b00+a21*b10+a22*b20+a23*b30;
	c[9]  = a20*b01+a21*b11+a22*b21+a23*b31;
	c[10] = a20*b02+a21*b12+a22*b22+a23*b32;
	c[11] = a20*b03+a21*b13+a22*b23+a23*b33;
	c[12] = a30*b00+a31*b10+a32*b20+a33*b30;
	c[13] = a30*b01+a31*b11+a32*b21+a33*b31;
	c[14] = a30*b02+a31*b12+a32*b22+a33*b32;
	c[15] = a30*b03+a31*b13+a32*b23+a33*b33;
}

void vml_mat4_inv(
	vml_real a[16],
	vml_real b[16]
) {
	vml_real a00 = a[0];
	vml_real a01 = a[1];
	vml_real a02 = a[2];
	vml_real a03 = a[3];
	vml_real a10 = a[4];
	vml_real a11 = a[5];
	vml_real a12 = a[6];
	vml_real a13 = a[7];
	vml_real a20 = a[8];
	vml_real a21 = a[9];
	vml_real a22 = a[10];
	vml_real a23 = a[11];
	vml_real a30 = a[12];
	vml_real a31 = a[13];
	vml_real a32 = a[14];
	vml_real a33 = a[15];

	vml_real b00 =  a11*a22*a33-a11*a23*a32-a21*a12*a33+a21*a13*a32+a31*a12*a23-a31*a13*a22;
	vml_real b01 = -a01*a22*a33+a01*a23*a32+a21*a02*a33-a21*a03*a32-a31*a02*a23+a31*a03*a22;
	vml_real b02 =  a01*a12*a33-a01*a13*a32-a11*a02*a33+a11*a03*a32+a31*a02*a13-a31*a03*a12;
	vml_real b03 = -a01*a12*a23+a01*a13*a22+a11*a02*a23-a11*a03*a22-a21*a02*a13+a21*a03*a12;
	vml_real b10 = -a10*a22*a33+a10*a23*a32+a20*a12*a33-a20*a13*a32-a30*a12*a23+a30*a13*a22;
	vml_real b11 =  a00*a22*a33-a00*a23*a32-a20*a02*a33+a20*a03*a32+a30*a02*a23-a30*a03*a22;
	vml_real b12 = -a00*a12*a33+a00*a13*a32+a10*a02*a33-a10*a03*a32-a30*a02*a13+a30*a03*a12;
	vml_real b13 =  a00*a12*a23-a00*a13*a22-a10*a02*a23+a10*a03*a22+a20*a02*a13-a20*a03*a12;
	vml_real b20 =  a10*a21*a33-a10*a23*a31-a20*a11*a33+a20*a13*a31+a30*a11*a23-a30*a13*a21;
	vml_real b21 = -a00*a21*a33+a00*a23*a31+a20*a01*a33-a20*a03*a31-a30*a01*a23+a30*a03*a21;
	vml_real b22 =  a00*a11*a33-a00*a13*a31-a10*a01*a33+a10*a03*a31+a30*a01*a13-a30*a03*a11;
	vml_real b23 = -a00*a11*a23+a00*a13*a21+a10*a01*a23-a10*a03*a21-a20*a01*a13+a20*a03*a11;
	vml_real b30 = -a10*a21*a32+a10*a22*a31+a20*a11*a32-a20*a12*a31-a30*a11*a22+a30*a12*a21;
	vml_real b31 =  a00*a21*a32-a00*a22*a31-a20*a01*a32+a20*a02*a31+a30*a01*a22-a30*a02*a21;
	vml_real b32 = -a00*a11*a32+a00*a12*a31+a10*a01*a32-a10*a02*a31-a30*a01*a12+a30*a02*a11;
	vml_real b33 =  a00*a11*a22-a00*a12*a21-a10*a01*a22+a10*a02*a21+a20*a01*a12-a20*a02*a11;

	vml_real det = 1/(a00*b00+a01*b10+a02*b20+a03*b30);

	b[0]  = b00*det;
	b[1]  = b01*det;
	b[2]  = b02*det;
	b[3]  = b03*det;
	b[4]  = b10*det;
	b[5]  = b11*det;
	b[6]  = b12*det;
	b[7]  = b13*det;
	b[8]  = b20*det;
	b[9]  = b21*det;
	b[10] = b22*det;
	b[11] = b23*det;
	b[12] = b30*det;
	b[13] = b31*det;
	b[14] = b32*det;
	b[15] = b33*det;
}

void vml_mat4_mat3_get(
	vml_real a[16],
	vml_real b[9]
) {
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
	b[3] = a[4];
	b[4] = a[5];
	b[5] = a[6];
	b[6] = a[8];
	b[7] = a[9];
	b[8] = a[10];
}

void vml_mat4_euler_set(
	vml_real a[16],
	vml_real b[3],
	vml_real c[16]
) {
	vml_real cx = cosf(b[0]);
	vml_real cy = cosf(b[1]);
	vml_real cz = cosf(b[2]);
	vml_real sx = sinf(b[0]);
	vml_real sy = sinf(b[1]);
	vml_real sz = sinf(b[2]);

	c[0]  = cy*cz;
	c[1]  = -cy*sz;
	c[2]  = sy;
	c[3]  = a[3];
	c[4]  = cz*sx*sy+cx*sz;
	c[5]  = cx*cz-sx*sy*sz;
	c[6]  = -cy*sx;
	c[7]  = a[7];
	c[8]  = sx*sz-cx*cz*sy;
	c[9]  = cz*sx+cx*sy*sz;
	c[10] = cx*cy;
	c[11] = a[11];
	c[12] = a[12];
	c[13] = a[13];
	c[14] = a[14];
	c[15] = a[15];
}

void vml_mat4_euler_get(
	vml_real a[16],
	vml_real b[3]
) {
	b[0] = atan2f(-a[6],a[10]);
	b[1] = asinf(a[2]);
	b[2] = atan2f(-a[1],a[0]);
}

void vml_mat4_quat_set(
	vml_real a[16],
	vml_real b[4],
	vml_real c[16]
) {
	vml_real x = b[0];
	vml_real y = b[1];
	vml_real z = b[2];
	vml_real w = b[3];

	vml_real xx = x*x;
	vml_real yy = y*y;
	vml_real zz = z*z;
	vml_real xy = x*y;
	vml_real xz = x*z;
	vml_real yz = y*z;
	vml_real xw = x*w;
	vml_real yw = y*w;
	vml_real zw = z*w;

	c[0]  = 1-2*yy-2*zz;
	c[1]  = 2*(xy-zw);
	c[2]  = 2*(xz+yw);
	c[3]  = a[3];
	c[4]  = 2*(xy+zw);
	c[5]  = 1-2*xx-2*zz;
	c[6]  = 2*(yz-xw);
	c[7]  = a[7];
	c[8]  = 2*(xz-yw);
	c[9]  = 2*(yz+xw);
	c[10] = 1-2*xx-2*yy;
	c[11] = a[11];
	c[12] = a[12];
	c[13] = a[13];
	c[14] = a[14];
	c[15] = a[15];
}

void vml_mat4_quat_get(
	vml_real a[16],
	vml_real b[4]
) {
	vml_real a00 = a[0];
	vml_real a01 = a[1];
	vml_real a02 = a[2];
	vml_real a10 = a[4];
	vml_real a11 = a[5];
	vml_real a12 = a[6];
	vml_real a20 = a[8];
	vml_real a21 = a[9];
	vml_real a22 = a[10];

	vml_real x = sqrtf(vml_max(( a00-a11-a22+1)/4,0));
	vml_real y = sqrtf(vml_max((-a00+a11-a22+1)/4,0));
	vml_real z = sqrtf(vml_max((-a00-a11+a22+1)/4,0));
	vml_real w = sqrtf(vml_max(( a00+a11+a22+1)/4,0));

	if (w>=x && w>=y && w>=z) {
		x = x*vml_sign(a21-a12);
		y = y*vml_sign(a02-a20);
		z = z*vml_sign(a10-a01);
	} else if (x>=w && x>=y && x>=z) {
		y = y*vml_sign(a10+a01);
		z = z*vml_sign(a02+a20);
		w = w*vml_sign(a21-a12);
	} else if (y>=w && y>=x && y>=z) {
		x = x*vml_sign(a10+a01);
		z = z*vml_sign(a21+a12);
		w = w*vml_sign(a02-a20);
	} else if (z>=w && z>=x && z>=y) {
		x = x*vml_sign(a20+a02);
		y = y*vml_sign(a21+a12);
		w = w*vml_sign(a10-a01);
	}

	vml_real m = 1.0f/sqrtf(x*x+y*y+z*z+w*w);

	b[0] = x*m;
	b[1] = y*m;
	b[2] = z*m;
	b[3] = w*m;
}

void vml_mat4_axis_set(
	vml_real a[16],
	vml_real b[4],
	vml_real c[16]
) {
	vml_real x = b[0];
	vml_real y = b[1];
	vml_real z = b[2];
	vml_real w = b[3];

	vml_real cw = cosf(w);
	vml_real sw = sinf(w);
	vml_real m  = 1.0f/sqrtf(x*x+y*y+z*z);

	x *= m;
	y *= m;
	z *= m;

	vml_real xx = x*x;
	vml_real yy = y*y;
	vml_real zz = z*z;

	c[0]  = cw+xx*(1-cw);
	c[1]  = yy*(1-cw)-z*sw;
	c[2]  = zz*(1-cw)+y*sw;
	c[3]  = a[3];
	c[4]  = xx*(1-cw)+z*sw;
	c[5]  = cw+yy*(1-cw);
	c[6]  = zz*(1-cw)-x*sw;
	c[7]  = a[7];
	c[8]  = x*z*(1-cw)-y*sw;
	c[9]  = y*z*(1-cw)+x*sw;
	c[10] = cw+zz*(1-cw);
	c[11] = a[11];
	c[12] = a[12];
	c[13] = a[13];
	c[14] = a[14];
	c[15] = a[15];
}

void vml_mat4_axis_get(
	vml_real a[16],
	vml_real b[4]
) {
	vml_real a21_a12 = a[9]-a[6];
	vml_real a02_a20 = a[2]-a[8];
	vml_real a10_a01 = a[4]-a[1];

	vml_real m=1.0f/sqrtf(
		a21_a12*a21_a12+
		a02_a20*a02_a20+
		a10_a01*a10_a01
	);

	b[0] = a21_a12*m;
	b[1] = a02_a20*m;
	b[2] = a10_a01*m;
	b[3] = acosf((a[0]+a[5]+a[10]-1)/2);
}

void vml_mat4_look_set(
	vml_real a[16],
	vml_real b[3],
	vml_real c[3],
	vml_real d[16]
) {
	vml_real fx = b[0];
	vml_real fy = b[1];
	vml_real fz = b[2];
	vml_real ux = c[0];
	vml_real uy = c[1];
	vml_real uz = c[2];
	vml_real rx = uy*fz-uz*fy;
	vml_real ry = uz*fx-ux*fz;
	vml_real rz = ux*fy-uy*fx;
	vml_real rm = 1.0f/sqrtf(rx*rx+ry*ry+rz*rz);

	rx *= rm;
	ry *= rm;
	rz *= rm;

	d[0]  = rx;
	d[1]  = fy*rz-fz*ry;
	d[2]  = fx;
	d[3]  = a[3];
	d[4]  = ry;
	d[5]  = fz*rx-fx*rz;
	d[6]  = fy;
	d[7]  = a[7];
	d[8]  = rz;
	d[9]  = fx*ry-fy*rx;
	d[10] = fz;
	d[11] = a[11];
	d[12] = a[12];
	d[13] = a[13];
	d[14] = a[14];
	d[15] = a[15];
}

void vml_mat4_look_get(
	vml_real a[16],
	vml_real b[3]
) {
	b[0] = a[2];
	b[1] = a[6];
	b[2] = a[10];
}

void vml_mat4_pos_set(
	vml_real a[16],
	vml_real b[3],
	vml_real c[16]
) {
	c[0]  = a[0];
	c[1]  = a[1];
	c[2]  = a[2];
	c[3]  = b[0];
	c[4]  = a[4];
	c[5]  = a[5];
	c[6]  = a[6];
	c[7]  = b[1];
	c[8]  = a[8];
	c[9]  = a[9];
	c[10] = a[10];
	c[11] = b[2];
	c[12] = a[12];
	c[13] = a[13];
	c[14] = a[14];
	c[15] = a[15];
}

void vml_mat4_pos_get(
	vml_real a[16],
	vml_real b[3]
) {
	b[0] = a[3];
	b[1] = a[7];
	b[2] = a[11];
}

void vml_mat4_scale_set(
	vml_real a[16],
	vml_real b[3],
	vml_real c[16]
) {
	vml_real rx = a[0];
	vml_real ry = a[4];
	vml_real rz = a[8];
	vml_real tx = a[1];
	vml_real ty = a[5];
	vml_real tz = a[9];
	vml_real fx = a[2];
	vml_real fy = a[6];
	vml_real fz = a[10];	

	vml_real rm = b[0]/sqrtf(rx*rx+ry*ry+rz*rz);
	vml_real tm = b[1]/sqrtf(tx*tx+ty*ty+tz*tz);
	vml_real fm = b[2]/sqrtf(fx*fx+fy*fy+fz*fz);

	c[0]  = rx*rm;
	c[1]  = tx*tm;
	c[2]  = fx*fm;
	c[3]  = a[3];
	c[4]  = ry*rm;
	c[5]  = ty*tm;
	c[6]  = fy*fm;
	c[4]  = a[4];
	c[8]  = rz*rm;
	c[9]  = tz*tm;
	c[10] = fz*fm;
	c[11] = a[11];
	c[12] = a[12];
	c[13] = a[13];
	c[14] = a[14];
	c[15] = a[15];
}

void vml_mat4_scale_get(
	vml_real a[16],
	vml_real b[3]
) {
	vml_real rx = a[0];
	vml_real ry = a[4];
	vml_real rz = a[8];
	vml_real tx = a[1];
	vml_real ty = a[5];
	vml_real tz = a[9];
	vml_real fx = a[2];
	vml_real fy = a[6];
	vml_real fz = a[10];	

	b[0] = sqrtf(rx*rx+ry*ry+rz*rz);
	b[1] = sqrtf(tx*tx+ty*ty+tz*tz);
	b[2] = sqrtf(fx*fx+fy*fy+fz*fz);
}

void vml_mat4_lerp(
	vml_real a[16],
	vml_real b[16],
	vml_real t,
	vml_real c[16]
) {
	vml_real aq[4];
	vml_real bq[4];
	vml_real cq[4];
	vml_real ap[3];
	vml_real bp[3];
	vml_real cp[3];

	vml_mat4_quat_get(a,aq);
	vml_mat4_quat_get(b,bq);

	vml_mat4_pos_get(a,ap);
	vml_mat4_pos_get(b,bp);

	vml_quat_lerp(aq,bq,t,cq);
	vml_vec3_lerp(ap,bp,t,cp);

	vml_mat4_quat_set(c,cq,c);
	vml_mat4_pos_set(c,cp,c);
}

void vml_mat4_perspective(
	vml_real v,
	vml_real r,
	vml_real n,
	vml_real f,
	vml_real a[16]
) {
	vml_real t = tanf(v/2);

	a[0]  = 1/(r*t);
	a[1]  = 0;
	a[2]  = 0;
	a[3]  = 0;
	a[4]  = 0;
	a[5]  = 1/t;
	a[6]  = 0;
	a[7]  = 0;
	a[8]  = 0;
	a[9]  = 0;
	a[10] = (f+n)/(n-f);
	a[11] = (2*f*n)/(n-f);
	a[12] = 0;
	a[13] = 0;
	a[14] = -1;
	a[15] = 0;
}

void vml_mat4_orthographic(
	vml_real l,
	vml_real r,
	vml_real t,
	vml_real b,
	vml_real n,
	vml_real f,
	vml_real a[16]
) {
	a[0] = 2/(r-l);
	a[1] = 0;
	a[2] = 0;
	a[3] = -(r+l)/(r-l);
	a[4] = 0;
	a[5] = 2/(t-b);
	a[6] = 0;
	a[7] = -(t+b)/(t-b);
	a[8] = 0;
	a[9] = 0;
	a[10] = -2/(f-n);
	a[11] = -(f+n)/(f-n);
	a[12] = 0;
	a[13] = 0;
	a[14] = 0;
	a[15] = 1;
}

#endif
#endif
