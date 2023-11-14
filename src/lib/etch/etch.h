#ifndef ETCH_H
#define ETCH_H

#define ETCH_VERSION         1
#define ETCH_BYTES_PER_PIXEL 4
#define ETCH_MAX_TEXTURES    256

typedef unsigned char etch_char;
typedef unsigned int  etch_uint;
typedef float         etch_float;

typedef struct {
	etch_uint   width;
	etch_uint   height;
	etch_char  *data;
	etch_float *z_data;
} etch_texture;

extern etch_texture *textures;

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
	etch_float d_ax,
	etch_float d_ay,
	etch_float c_ar,
	etch_float c_ag,
	etch_float c_ab,
	etch_float c_aa
);

void etch_texture_rectangle_draw(
	etch_uint d_texture_id,
	etch_uint s_texture_id,
	etch_float d_ax,
	etch_float d_ay,
	etch_float d_bx,
	etch_float d_by,
	etch_float s_ax,
	etch_float s_ay,
	etch_float s_bx,
	etch_float s_by,
	etch_float c_ar,
	etch_float c_ag,
	etch_float c_ab,
	etch_float c_aa
);

void etch_texture_triangle_draw(
	etch_uint d_texture_id,
	etch_uint s_texture_id,
	etch_float d_ax,
	etch_float d_ay,
	etch_float d_az,
	etch_float d_bx,
	etch_float d_by,
	etch_float d_bz,
	etch_float d_cx,
	etch_float d_cy,
	etch_float d_cz,
	etch_float s_ax,
	etch_float s_ay,
	etch_float s_bx,
	etch_float s_by,
	etch_float s_cx,
	etch_float s_cy,
	etch_float c_ar,
	etch_float c_ag,
	etch_float c_ab,
	etch_float c_aa,
	etch_float c_br,
	etch_float c_bg,
	etch_float c_bb,
	etch_float c_ba,
	etch_float c_cr,
	etch_float c_cg,
	etch_float c_cb,
	etch_float c_ca
);

void etch_texture_flush();

etch_uint etch_texture_init();

void etch_texture_deinit();

#endif