#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/dmesa.h>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/stb_image.h"

#define MAX_TEXTURES 256
#define MAX_VERTEXES 256

static DMesaVisual  visual;
static DMesaContext context;
static DMesaBuffer  buffer;

static GLuint *textures;

ice_uint ice_video_texture_new(
	ice_uint width,
	ice_uint height,
	ice_uint attributes
) {
	
	ice_log((ice_char *)"Invalid texture format");
	
	return 0;
}

ice_uint ice_video_texture_load(
	ice_uint file_id
) {
	return 0;
}

void ice_video_texture_delete(
	ice_uint texture_id
) {
}

ice_uint ice_video_texture_width_get(
	ice_uint texture_id
) {
	return 0;
}

ice_uint ice_video_texture_height_get(
	ice_uint texture_id
) {
	return 0;
}

void ice_video_texture_clear(
	ice_uint texture_id
) {
}

void ice_video_texture_rectangle_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_real d_ax,
	ice_real d_ay,
	ice_real d_bx,
	ice_real d_by,
	ice_real s_ax,
	ice_real s_ay,
	ice_real s_bx,
	ice_real s_by,
	ice_real c_ar,
	ice_real c_ag,
	ice_real c_ab,
	ice_real c_aa
) {
	glBegin(GL_POLYGON);
	glColor3f(
		(GLfloat)c_ar,
		(GLfloat)c_ag,
		(GLfloat)c_ab
	);
	
	glEnd();
	glFlush();
}

void ice_video_texture_triangle_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_real d_ax,
	ice_real d_ay,
	ice_real d_az,
	ice_real d_bx,
	ice_real d_by,
	ice_real d_bz,
	ice_real d_cx,
	ice_real d_cy,
	ice_real d_cz,
	ice_real s_ax,
	ice_real s_ay,
	ice_real s_bx,
	ice_real s_by,
	ice_real s_cx,
	ice_real s_cy,
	ice_real c_ar,
	ice_real c_ag,
	ice_real c_ab,
	ice_real c_aa,
	ice_real c_br,
	ice_real c_bg,
	ice_real c_bb,
	ice_real c_ba,
	ice_real c_cr,
	ice_real c_cg,
	ice_real c_cb,
	ice_real c_ca
) {
	if (context==NULL) {
		return;
	}
	
	glBegin(GL_POLYGON);
	glColor3f(
		(GLfloat)c_ar,
		(GLfloat)c_ag,
		(GLfloat)c_ab
	);
	glVertex3f(
		(GLfloat)d_ax*2-1,
		1-(GLfloat)d_ay*2,
		(GLfloat)d_az
	);
	glColor3f(
		(GLfloat)c_br,
		(GLfloat)c_bg,
		(GLfloat)c_bb
	);
	glVertex3f(
		(GLfloat)d_bx*2-1,
		1-(GLfloat)d_by*2,
		(GLfloat)d_bz
	);
	glColor3f(
		(GLfloat)c_cr,
		(GLfloat)c_cg,
		(GLfloat)c_cb
	);
	glVertex3f(
		(GLfloat)d_cx*2-1,
		1-(GLfloat)d_cy*2,
		(GLfloat)d_cz
	);
	glEnd();
	glFlush();
}

ice_uint ice_video_vertex_new(
	ice_uint size
) {
	return 0;
}

void ice_video_vertex_delete(
	ice_uint vertex_id
) {
}

ice_uint ice_video_vertex_size_get(
	ice_uint vertex_id
) {
	return 0;
}

void ice_video_vertex_set_uint(
	ice_uint vertex_id,
	ice_uint index,
	ice_uint value
) {
}

ice_uint ice_video_vertex_get_uint(
	ice_uint vertex_id,
	ice_uint index
) {
	return 0;
}

void ice_video_vertex_set_sint(
	ice_uint vertex_id,
	ice_uint index,
	ice_sint value
) {
}

ice_sint ice_video_vertex_get_sint(
	ice_uint vertex_id,
	ice_uint index
) {
	return 0;
}

void ice_video_vertex_set_real(
	ice_uint vertex_id,
	ice_uint index,
	ice_real value
) {
}

ice_real ice_video_vertex_get_real(
	ice_uint vertex_id,
	ice_uint index
) {
	return 0;
}

void ice_video_vertex_texture_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_real render_matrix[4][4],
	ice_uint position_vertex_id,
	ice_uint texture_vertex_id,
	ice_uint face_vertex_id
) {
}

void ice_video_texture_flush() {
}

void ice_video_vertex_flush() {
}

void ice_video_buffer() {
	static DMesaBuffer buffer;
	
	DMesaSwapBuffers(buffer);
}

ice_uint ice_video_init(
	ice_uint width,
	ice_uint height
) {
	visual = DMesaCreateVisual(
		640, // X res
		480, // Y res
		16,  // BPP
		0,   // refresh rate: 0=default
		0,   // double-buffered
		1,   // RGB mode
		0,   // requested bits/alpha
		16,  // requested bits/depth
		0,   // requested bits/stencil
		0    // requested bits/accum
	);
	
	context = DMesaCreateContext(
		visual,
		NULL
	);
	
	buffer = DMesaCreateBuffer(
		visual,
		0,   // X pos
		0,   // Y pos
		640, // X res
		480  // Y res
	);
	
	DMesaMakeCurrent(
		context,
		buffer
	);
	
	glGenTextures(
		MAX_TEXTURES,
		textures
	); 
	
	return 0;
}

void ice_video_deinit() {	
	glDeleteTextures(
		MAX_TEXTURES,
		textures
	);
	
	DMesaDestroyBuffer(buffer);
	DMesaDestroyContext(context);
	DMesaDestroyVisual(visual);
}