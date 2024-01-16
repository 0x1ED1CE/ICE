#ifndef ETCH_H
#define ETCH_H

#define ETCH_VERSION 1

#define ETCH_MAX_TEXTURES 256
#define ETCH_MAX_VERTEXES 256

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
	etch_char r;
	etch_char g;
	etch_char b;
	etch_char a;
} etch_pixel;

typedef struct {
	etch_sint x;
	etch_sint y;
} etch_sint_vector2;

typedef struct {
	etch_sint x;
	etch_sint y;
	etch_sint z;
} etch_sint_vector3;

typedef struct {
	etch_real x;
	etch_real y;
	etch_real z;
} etch_real_vector3;

typedef struct {
	etch_uint   width;
	etch_uint   height;
	etch_pixel *data;
	etch_sint  *z_data;
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
	etch_sint_vector2 d_a,
	etch_pixel c_a
);

void etch_texture_rectangle_draw(
	etch_uint d_texture_id,
	etch_uint s_texture_id,
	etch_sint_vector2 d_a,
	etch_sint_vector2 d_b,
	etch_sint_vector2 s_a,
	etch_sint_vector2 s_b,
	etch_pixel c_a
);

void etch_texture_triangle_draw(
	etch_uint d_texture_id,
	etch_uint s_texture_id,
	etch_sint_vector3 d_a,
	etch_sint_vector3 d_b,
	etch_sint_vector3 d_c,
	etch_sint_vector2 s_a,
	etch_sint_vector2 s_b,
	etch_sint_vector2 s_c,
	etch_pixel c_a,
	etch_pixel c_b,
	etch_pixel c_c
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