#include <math.h>

#ifndef LMATH_MATRIX
#define LMATH_MATRIX

typedef float lmath_real;

#define LMATH_MATRIX_44_IDENTITY_TEMPLATE { \
	{1.0, 0.0, 0.0, 0.0}, \
	{0.0, 1.0, 0.0, 0.0}, \
	{0.0, 0.0, 1.0, 0.0}, \
	{0.0, 0.0, 0.0, 1.0} \
}

void lmath_matrix_44_copy(
	lmath_real m0[4][4], //Source
	lmath_real m1[4][4]  //Destination
) {
	m1[0][0] = m0[0][0];
	m1[0][1] = m0[0][1];
	m1[0][2] = m0[0][2];
	m1[0][3] = m0[0][3];
	m1[1][0] = m0[1][0];
	m1[1][1] = m0[1][1];
	m1[1][2] = m0[1][2];
	m1[1][3] = m0[1][3];
	m1[2][0] = m0[2][0];
	m1[2][1] = m0[2][1];
	m1[2][2] = m0[2][2];
	m1[2][3] = m0[2][3];
	m1[3][0] = m0[3][0];
	m1[3][1] = m0[3][1];
	m1[3][2] = m0[3][2];
	m1[3][3] = m0[3][3];
}

void lmath_matrix_44_multiply(
	lmath_real m0[4][4],
	lmath_real m1[4][4]
) {
	lmath_real a00 = m0[0][0]; lmath_real b00 = m1[0][0];
	lmath_real a01 = m0[0][1]; lmath_real b01 = m1[0][1];
	lmath_real a02 = m0[0][2]; lmath_real b02 = m1[0][2];
	lmath_real a03 = m0[0][3]; lmath_real b03 = m1[0][3];
	lmath_real a10 = m0[1][0]; lmath_real b10 = m1[1][0];
	lmath_real a11 = m0[1][1]; lmath_real b11 = m1[1][1];
	lmath_real a12 = m0[1][2]; lmath_real b12 = m1[1][2];
	lmath_real a13 = m0[1][3]; lmath_real b13 = m1[1][3];
	lmath_real a20 = m0[2][0]; lmath_real b20 = m1[2][0];
	lmath_real a21 = m0[2][1]; lmath_real b21 = m1[2][1];
	lmath_real a22 = m0[2][2]; lmath_real b22 = m1[2][2];
	lmath_real a23 = m0[2][3]; lmath_real b23 = m1[2][3];
	lmath_real a30 = m0[3][0]; lmath_real b30 = m1[3][0];
	lmath_real a31 = m0[3][1]; lmath_real b31 = m1[3][1];
	lmath_real a32 = m0[3][2]; lmath_real b32 = m1[3][2];
	lmath_real a33 = m0[3][3]; lmath_real b33 = m1[3][3];
	
	m0[0][0] = a00*b00+a01*b10+a02*b20+a03*b30;
	m0[0][1] = a00*b01+a01*b11+a02*b21+a03*b31;
	m0[0][2] = a00*b02+a01*b12+a02*b22+a03*b32;
	m0[0][3] = a00*b03+a01*b13+a02*b23+a03*b33;
	m0[1][0] = a10*b00+a11*b10+a12*b20+a13*b30;
	m0[1][1] = a10*b01+a11*b11+a12*b21+a13*b31;
	m0[1][2] = a10*b02+a11*b12+a12*b22+a13*b32;
	m0[1][3] = a10*b03+a11*b13+a12*b23+a13*b33;
	m0[2][0] = a20*b00+a21*b10+a22*b20+a23*b30;
	m0[2][1] = a20*b01+a21*b11+a22*b21+a23*b31;
	m0[2][2] = a20*b02+a21*b12+a22*b22+a23*b32;
	m0[2][3] = a20*b03+a21*b13+a22*b23+a23*b33;
	m0[3][0] = a30*b00+a31*b10+a32*b20+a33*b30;
	m0[3][1] = a30*b01+a31*b11+a32*b21+a33*b31;
	m0[3][2] = a30*b02+a31*b12+a32*b22+a33*b32;
	m0[3][3] = a30*b03+a31*b13+a32*b23+a33*b33;
}

void lmath_matrix_44_inverse(
	lmath_real m0[4][4]
) {
	lmath_real a00 = m0[0][0];
	lmath_real a01 = m0[0][1];
	lmath_real a02 = m0[0][2];
	lmath_real a03 = m0[0][3];
	lmath_real a10 = m0[1][0];
	lmath_real a11 = m0[1][1];
	lmath_real a12 = m0[1][2];
	lmath_real a13 = m0[1][3];
	lmath_real a20 = m0[2][0];
	lmath_real a21 = m0[2][1];
	lmath_real a22 = m0[2][2];
	lmath_real a23 = m0[2][3];
	lmath_real a30 = m0[3][0];
	lmath_real a31 = m0[3][1];
	lmath_real a32 = m0[3][2];
	lmath_real a33 = m0[3][3];
	
	lmath_real b00 =  a11*a22*a33-a11*a23*a32-a21*a12*a33+a21*a13*a32+a31*a12*a23-a31*a13*a22;
	lmath_real b01 = -a01*a22*a33+a01*a23*a32+a21*a02*a33-a21*a03*a32-a31*a02*a23+a31*a03*a22;
	lmath_real b02 =  a01*a12*a33-a01*a13*a32-a11*a02*a33+a11*a03*a32+a31*a02*a13-a31*a03*a12;
	lmath_real b03 = -a01*a12*a23+a01*a13*a22+a11*a02*a23-a11*a03*a22-a21*a02*a13+a21*a03*a12;
	lmath_real b10 = -a10*a22*a33+a10*a23*a32+a20*a12*a33-a20*a13*a32-a30*a12*a23+a30*a13*a22;
	lmath_real b11 =  a00*a22*a33-a00*a23*a32-a20*a02*a33+a20*a03*a32+a30*a02*a23-a30*a03*a22;
	lmath_real b12 = -a00*a12*a33+a00*a13*a32+a10*a02*a33-a10*a03*a32-a30*a02*a13+a30*a03*a12;
	lmath_real b13 =  a00*a12*a23-a00*a13*a22-a10*a02*a23+a10*a03*a22+a20*a02*a13-a20*a03*a12;
	lmath_real b20 =  a10*a21*a33-a10*a23*a31-a20*a11*a33+a20*a13*a31+a30*a11*a23-a30*a13*a21;
	lmath_real b21 = -a00*a21*a33+a00*a23*a31+a20*a01*a33-a20*a03*a31-a30*a01*a23+a30*a03*a21;
	lmath_real b22 =  a00*a11*a33-a00*a13*a31-a10*a01*a33+a10*a03*a31+a30*a01*a13-a30*a03*a11;
	lmath_real b23 = -a00*a11*a23+a00*a13*a21+a10*a01*a23-a10*a03*a21-a20*a01*a13+a20*a03*a11;
	lmath_real b30 = -a10*a21*a32+a10*a22*a31+a20*a11*a32-a20*a12*a31-a30*a11*a22+a30*a12*a21;
	lmath_real b31 =  a00*a21*a32-a00*a22*a31-a20*a01*a32+a20*a02*a31+a30*a01*a22-a30*a02*a21;
	lmath_real b32 = -a00*a11*a32+a00*a12*a31+a10*a01*a32-a10*a02*a31-a30*a01*a12+a30*a02*a11;
	lmath_real b33 =  a00*a11*a22-a00*a12*a21-a10*a01*a22+a10*a02*a21+a20*a01*a12-a20*a02*a11;
	
	lmath_real det = a00*b00+a01*b10+a02*b20+a03*b30;
	
	m0[0][0] = b00/det;
	m0[0][1] = b01/det;
	m0[0][2] = b02/det;
	m0[0][3] = b03/det;
	m0[1][0] = b10/det;
	m0[1][1] = b11/det;
	m0[1][2] = b12/det;
	m0[1][3] = b13/det;
	m0[2][0] = b20/det;
	m0[2][1] = b21/det;
	m0[2][2] = b22/det;
	m0[2][3] = b23/det;
	m0[3][0] = b30/det;
	m0[3][1] = b31/det;
	m0[3][2] = b32/det;
	m0[3][3] = b33/det;
}

void lmath_matrix_44_identity(
	lmath_real m0[4][4]
) {
	lmath_real m1[4][4]=LMATH_MATRIX_44_IDENTITY_TEMPLATE;
	lmath_matrix_44_copy(m1,m0);
}

void lmath_matrix_44_perspective(
	lmath_real m0[4][4],
	lmath_real fov,
	lmath_real ratio,
	lmath_real near,
	lmath_real far
) {
	lmath_real t = (lmath_real)tanf((float)fov/2);
	
	m0[0][0] = 1/(t*ratio);
	m0[0][1] = 0;
	m0[0][2] = 0;
	m0[0][3] = 0;
	m0[1][0] = 0;
	m0[1][1] = 1/t;
	m0[1][2] = 0;
	m0[1][3] = 0;
	m0[2][0] = 0;
	m0[2][1] = 0;
	m0[2][2] = -(far+near)/(far-near);
	m0[2][3] = -(2*far*near)/(far-near);
	m0[3][0] = 0;
	m0[3][1] = 0;
	m0[3][2] = -1;
	m0[3][3] = 0;
}

void lmath_matrix_44_position_set(
	lmath_real m0[4][4],
	lmath_real v0[3]
) {
	m0[0][3] = v0[0];
	m0[1][3] = v0[1];
	m0[2][3] = v0[2];
}

void lmath_matrix_44_position_get(
	lmath_real m0[4][4],
	lmath_real v0[3]
) {
	v0[0] = m0[0][3];
	v0[1] = m0[1][3];
	v0[2] = m0[2][3];
}

void lmath_matrix_44_transform(
	lmath_real m0[4][4],
	lmath_real v0[3]
) {
	lmath_real a00 = m0[0][0];
	lmath_real a01 = m0[0][1];
	lmath_real a02 = m0[0][2];
	lmath_real a03 = m0[0][3];
	lmath_real a10 = m0[1][0];
	lmath_real a11 = m0[1][1];
	lmath_real a12 = m0[1][2];
	lmath_real a13 = m0[1][3];
	lmath_real a20 = m0[2][0];
	lmath_real a21 = m0[2][1];
	lmath_real a22 = m0[2][2];
	lmath_real a23 = m0[2][3];
	
	lmath_real b0 = v0[0];
	lmath_real b1 = v0[1];
	lmath_real b2 = v0[2];
	
	m0[0][3] = a03+b0*a00+b1*a01+b2*a02;
	m0[1][3] = a13+b0*a10+b1*a11+b2*a12;
	m0[2][3] = a23+b0*a20+b1*a21+b2*a22;
}


void lmath_matrix_44_euler_set(
	lmath_real m0[4][4],
	lmath_real e0[3]
) {
	lmath_real cx = cosf((float)e0[0]);
	lmath_real sx = sinf((float)e0[0]);
	lmath_real cy = cosf((float)e0[1]);
	lmath_real sy = sinf((float)e0[1]);
	lmath_real cz = cosf((float)e0[2]);
	lmath_real sz = sinf((float)e0[2]);
	
	m0[0][0] = cy*cz;
	m0[0][1] = -cy*sz;
	m0[0][2] = sy;
	m0[1][0] = cz*sx*sy+cx*sz;
	m0[1][1] = cx*cz-sx*sy*sz;
	m0[1][2] = -cy*sx;
	m0[2][0] = sx*sz-cx*cz*sy;
	m0[2][1] = cz*sx+cx*sy*sz;
	m0[2][2] = cx*cy;
}

void lmath_matrix_44_euler_get(
	lmath_real m0[4][4],
	lmath_real e0[3]
) {
	e0[0] = atan2f(-m0[1][2],m0[2][2]);
	e0[1] = asinf(m0[0][2]);
	e0[2] = atan2f(-m0[0][1],m0[0][0]);
}

void lmath_matrix_44_rotate_euler(
	lmath_real m0[4][4],
	lmath_real e0[3]
) {
	lmath_real m1[4][4] = LMATH_MATRIX_44_IDENTITY_TEMPLATE;
	
	lmath_matrix_44_euler_set(m1,e0);
	lmath_matrix_44_multiply(m0,m1);
}

void lmath_matrix_44_look_get(
	lmath_real m0[4][4],
	lmath_real v0[3]
) {
	v0[0] = m0[0][2];
	v0[1] = m0[1][2];
	v0[2] = m0[2][2];
}

#endif