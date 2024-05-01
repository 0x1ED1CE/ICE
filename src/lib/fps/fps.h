/**********************************[LICENSE]***********************************

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

******************************************************************************/

#ifndef FPS_H
#define FPS_H

#define FPS_VERSION 1

/********************************[BASE TYPES]*********************************/

typedef unsigned char fps_char;
typedef unsigned int  fps_uint;
typedef signed int    fps_sint;
typedef float         fps_real;

/******************************[MATH FUNCTIONS]*******************************/

void fps_vector3_zero(
	fps_real *v0
);

void fps_vector3_copy(
	fps_real *v0,
	fps_real *v1
);

void fps_vector3_dot(
	fps_real *v0,
	fps_real *n0
);

void fps_vector3_cross(
	fps_real *v0,
	fps_real *v1,
	fps_real *v2
);

void fps_vector3_magnitude(
	fps_real *v0,
	fps_real *n0
);

void fps_vector3_normal(
	fps_real *v0,
	fps_real *v1
);

void fps_matrix44_identity(
	fps_real *m0
);

void fps_matrix44_copy(
	fps_real *m0,
	fps_real *m1
);

void fps_matrix44_matrix44_multiply(
	fps_real *m0,
	fps_real *m1,
	fps_real *m2
);

void fps_matrix44_vector3_multiply(
	fps_real *m0,
	fps_real *v0,
	fps_real *v1
);

void fps_matrix44_inverse(
	fps_real *m0,
	fps_real *m1
);

/********************************[SHAPE CLASS]********************************/

typedef struct {
} fps_shape;

/******************************[COLLIDER CLASS]*******************************/

typedef struct {
	fps_real density;
	fps_real friction;
	fps_real restitution;
	fps_real size[3];
	fps_real local_transform[16];
	fps_real global_transform[16];
} fps_collider;

/********************************[BODY CLASS]*********************************/

#define BODY_STATE_STATIC 1
#define BODY_STATE_SOLID  2
#define BODY_STATE_SLEEP  3

typedef struct {
	fps_uint state;
	fps_real total_mass;
	fps_real velocity[3];
	fps_real angular_velocity[3];
	fps_real force[3];
	fps_real torque[3];
	fps_real post_translation[3];
	fps_real transform[16];
	fps_real inverse_inertia[9];
} fps_body;

void fps_body_init(fps_body *body);

fps_char fps_body_state_get(
	fps_body *body,
	fps_uint flag
);

void fps_body_state_set(
	fps_body *body,
	fps_uint flag,
	fps_char state
);

/**************************[NGC COLLISION ALGORITHM]**************************/



/*****************************[RIGID BODY SOLVER]*****************************/

#endif