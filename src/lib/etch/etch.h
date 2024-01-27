#ifndef ETCH_H
#define ETCH_H

#define ETCH_VERSION 2

#define ETCH_MAX_TEXTURES 256
#define ETCH_MAX_VERTEXES 256

#define ETCH_PIXEL_SIZE 4

typedef unsigned char etch_char;
typedef unsigned int  etch_uint;
typedef signed int    etch_sint;
typedef float         etch_real;

typedef union {
	etch_uint uint;
	etch_sint sint;
	etch_real real;
} etch_word;

typedef struct {
	etch_uint  width;
	etch_uint  height;
	etch_char *data;
	etch_sint *z_data;
} etch_texture;

typedef struct {
	etch_uint size;
	etch_word *data;
} etch_vertex;

extern etch_texture *textures;

extern etch_vertex *vertexes;

etch_uint etch_texture_new(
	etch_uint width,
	etch_uint height,
	etch_char z_enable
);

void etch_texture_free(
	etch_uint texture_id
);

etch_uint etch_texture_width_get(
	etch_uint texture_id
);

etch_uint etch_texture_height_get(
	etch_uint texture_id
);

void etch_texture_clear(
	etch_uint texture_id
);

void etch_texture_pixel_draw(
	etch_uint d_texture_id,
	etch_sint d_ax,
	etch_sint d_ay,
	etch_char c_ar,
	etch_char c_ag,
	etch_char c_ab,
	etch_char c_aa
);

void etch_texture_rectangle_draw(
	etch_uint d_texture_id,
	etch_uint s_texture_id,
	etch_sint d_ax,
	etch_sint d_ay,
	etch_sint d_bx,
	etch_sint d_by,
	etch_sint s_ax,
	etch_sint s_ay,
	etch_sint s_bx,
	etch_sint s_by,
	etch_char c_ar,
	etch_char c_ag,
	etch_char c_ab,
	etch_char c_aa
);

void etch_texture_triangle_draw(
	etch_uint d_texture_id,
	etch_uint s_texture_id,
	etch_sint d_ax,
	etch_sint d_ay,
	etch_sint d_az,
	etch_sint d_bx,
	etch_sint d_by,
	etch_sint d_bz,
	etch_sint d_cx,
	etch_sint d_cy,
	etch_sint d_cz,
	etch_sint s_ax,
	etch_sint s_ay,
	etch_sint s_bx,
	etch_sint s_by,
	etch_sint s_cx,
	etch_sint s_cy,
	etch_char c_ar,
	etch_char c_ag,
	etch_char c_ab,
	etch_char c_aa,
	etch_char c_br,
	etch_char c_bg,
	etch_char c_bb,
	etch_char c_ba,
	etch_char c_cr,
	etch_char c_cg,
	etch_char c_cb,
	etch_char c_ca
);

void etch_texture_flush();

etch_uint etch_texture_init();

void etch_texture_deinit();

etch_uint etch_vertex_new(
	etch_uint size
);

void etch_vertex_free(
	etch_uint vertex_id
);

etch_uint etch_vertex_size_get(
	etch_uint vertex_id
);

void etch_vertex_set_uint(
	etch_uint vertex_id,
	etch_uint index,
	etch_uint value
);

etch_uint etch_vertex_get_uint(
	etch_uint vertex_id,
	etch_uint index
);

void etch_vertex_set_sint(
	etch_uint vertex_id,
	etch_uint index,
	etch_sint value
);

etch_sint etch_vertex_get_sint(
	etch_uint vertex_id,
	etch_uint index
);

void etch_vertex_set_real(
	etch_uint vertex_id,
	etch_uint index,
	etch_real value
);

etch_real etch_vertex_get_real(
	etch_uint vertex_id,
	etch_uint index
);

void etch_vertex_texture_draw(
	etch_uint d_texture_id,
	etch_uint s_texture_id,
	etch_real render_matrix[4][4],
	etch_uint position_vertex_id,
	etch_uint texture_vertex_id,
	etch_uint face_vertex_id
);

void etch_vertex_flush();

etch_uint etch_vertex_init();

void etch_vertex_deinit();

#endif