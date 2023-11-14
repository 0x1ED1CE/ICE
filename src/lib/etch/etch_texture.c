#include <malloc.h>
#include <string.h>
#include <math.h>

#include "etch.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

etch_texture *textures=NULL;

etch_uint etch_texture_new(
	etch_uint width,
	etch_uint height,
	etch_char z_enable
) {
	if (textures==NULL) {
		return 0;
	}
	
	etch_uint texture_id   = 0;
	etch_texture *texture  = NULL;
	etch_uint texture_size = width*height;
	
	for (etch_uint i=0; i<ETCH_MAX_TEXTURES; i++) {
		if (textures[i].data==NULL) {
			texture_id = i;
			texture    = &textures[i];
			
			break;
		}
	}
	
	if (texture==NULL) {
		return 0;
	}
	
	texture->width  = width;
	texture->height = height;
	texture->data   = (etch_char*)calloc(
		texture_size,
		ETCH_BYTES_PER_PIXEL
	);
	
	if (z_enable) {
		texture->z_data=(etch_float*)malloc(
			texture_size*sizeof(etch_float)
		);
		
		for (etch_uint i=0; i<texture_size; i++) {
			texture->z_data[i]=1;
		}
	}
	
	return texture_id;
}

void etch_texture_free(
	etch_uint texture_id
) {
	if (
		textures==NULL ||
		texture_id>=ETCH_MAX_TEXTURES
	) {
		return;
	}
	
	etch_texture *texture=&textures[texture_id];
	
	if (texture->data!=NULL) {
		free(texture->data);
		texture->data=NULL;
	}
	
	if (texture->z_data!=NULL) {
		free(texture->z_data);
		texture->z_data=NULL;
	}
	
	texture->width  = 0;
	texture->height = 0;
}

etch_uint etch_texture_width_get(
	etch_uint texture_id
) {
	if (
		textures==NULL ||
		texture_id>=ETCH_MAX_TEXTURES
	) {
		return 0;
	}
	
	etch_texture *texture=&textures[texture_id];
	
	return texture->width;
}

etch_uint etch_texture_height_get(
	etch_uint texture_id
) {
	if (
		textures==NULL ||
		texture_id>=ETCH_MAX_TEXTURES
	) {
		return 0;
	}
	
	etch_texture *texture=&textures[texture_id];
	
	return texture->height;
}

void etch_texture_clear(
	etch_uint texture_id
) {
	if (
		textures==NULL ||
		texture_id>=ETCH_MAX_TEXTURES
	) {
		return;
	}
	
	etch_texture *texture      = &textures[texture_id];
	etch_uint     texture_size = texture->width*texture->height;
	
	etch_char  *data   = texture->data;
	etch_float *z_data = texture->z_data;
	
	if (data!=NULL) {
		memset(
			data,
			0,
			texture_size*ETCH_BYTES_PER_PIXEL
		);
	}
	
	if (z_data!=NULL) {
		for (etch_uint i=0; i<texture_size; i++) {
			z_data[i]=1.0;
		}
	}
}

static inline void blend(
	etch_char ar,
	etch_char ag,
	etch_char ab,
	etch_char aa,
	etch_char br,
	etch_char bg,
	etch_char bb,
	etch_char ba,
	etch_char *cr,
	etch_char *cg,
	etch_char *cb,
	etch_char *ca
) {
	etch_uint alpha     = aa+1;
	etch_uint inv_alpha = 256-aa;
	*cr = (etch_char)((alpha*ar+inv_alpha*br)>>8);
	*cg = (etch_char)((alpha*ag+inv_alpha*bg)>>8);
	*cb = (etch_char)((alpha*ab+inv_alpha*bb)>>8);
	*ca = (etch_char)((alpha*aa+inv_alpha*ba)>>8);
}

void etch_texture_pixel_draw(
	etch_uint d_texture_id,
	etch_float d_ax,
	etch_float d_ay,
	etch_float c_ar,
	etch_float c_ag,
	etch_float c_ab,
	etch_float c_aa
) {
	if (
		textures==NULL ||
		d_texture_id>=ETCH_MAX_TEXTURES ||
		textures[d_texture_id].data==NULL
	) {
		return;
	}
	
	etch_texture *d_texture = &textures[d_texture_id];
	
	etch_char *d_data   = d_texture->data;
	etch_float d_width  = (etch_float)d_texture->width;
	etch_float d_height = (etch_float)d_texture->height;
	
	d_ax *= d_width-1;
	d_ay *= d_height-1;
	
	if (
		d_ax>=d_width ||
		d_ax<0 ||
		d_ay>=d_height ||
		d_ay<0
	) {
		return;
	}
	
	etch_uint d_index = (etch_uint)(d_ay*d_width+d_ax)*4;
	
	blend(
		(etch_char)(c_ar*255),
		(etch_char)(c_ag*255),
		(etch_char)(c_ab*255),
		(etch_char)(c_aa*255),
		d_data[d_index],
		d_data[d_index+1],
		d_data[d_index+2],
		d_data[d_index+3],
		&d_data[d_index],
		&d_data[d_index+1],
		&d_data[d_index+2],
		&d_data[d_index+3]
	);
}

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
) {
	if (
		textures==NULL ||
		d_texture_id>=ETCH_MAX_TEXTURES ||
		textures[d_texture_id].data==NULL
	) {
		return;
	}
	
	etch_texture *d_texture = &textures[d_texture_id];
	etch_texture *s_texture = &textures[s_texture_id];
	
	etch_char *d_data   = d_texture->data;
	etch_float d_width  = (etch_float)d_texture->width;
	etch_float d_height = (etch_float)d_texture->height;
	
	etch_char *s_data   = (etch_char[4]){255,255,255,255};
	etch_float s_width  = 1;
	etch_float s_height = 1;
	
	if (
		s_texture_id<ETCH_MAX_TEXTURES &&
		s_texture->data!=NULL
	) {
		s_data   = s_texture->data;
		s_width  = (etch_float)s_texture->width;
		s_height = (etch_float)s_texture->height;
	}
	
	d_ax *= d_width-1;
	d_ay *= d_height-1;
	d_bx *= d_width-1;
	d_by *= d_height-1;
	s_ax *= s_width-1;
	s_ay *= s_height-1;
	s_bx *= s_width-1;
	s_by *= s_height-1;
	
	etch_float min_x = MIN(d_ax,d_bx);
	etch_float min_y = MIN(d_ay,d_by);
	etch_float max_x = MAX(d_ax,d_bx);
	etch_float max_y = MAX(d_ay,d_by);
	
	if (
		min_x>=d_width ||
		max_x<0 ||
		min_y>=d_height ||
		max_y<0
	) {
		return;
	}
	
	min_x = MAX(min_x,0);
	min_y = MAX(min_y,0);
	max_x = MIN(max_x,d_width-1);
	max_y = MIN(max_y,d_height-1);
	
	etch_float scale_x = (s_bx-s_ax)/(d_bx-d_ax);
	etch_float scale_y = (s_by-s_ay)/(d_by-d_ay);
	etch_float r;
	etch_float g;
	etch_float b;
	etch_float a;
	etch_float d_x;
	etch_float d_y;
	etch_float s_x;
	etch_float s_y;
	etch_uint d_index;
	etch_uint s_index;
	
	for (d_y=min_y; d_y<max_y; d_y++) {
		for (d_x=min_x; d_x<max_x; d_x++) {
			s_x = fmodf(s_ax+(d_x-d_ax)*scale_x,s_width);
			s_y = fmodf(s_ay+(d_y-d_ay)*scale_y,s_height);
			
			d_index = (etch_uint)(d_y*d_width+d_x)*ETCH_BYTES_PER_PIXEL;
			s_index = (etch_uint)(s_y*s_width+s_x)*ETCH_BYTES_PER_PIXEL;
			
			r = (etch_float)s_data[s_index]/255*c_ar;
			g = (etch_float)s_data[s_index+1]/255*c_ag;
			b = (etch_float)s_data[s_index+2]/255*c_ab;
			a = (etch_float)s_data[s_index+3]/255*c_aa;
			
			blend(
				(etch_char)(r*255),
				(etch_char)(g*255),
				(etch_char)(b*255),
				(etch_char)(a*255),
				d_data[d_index],
				d_data[d_index+1],
				d_data[d_index+2],
				d_data[d_index+3],
				&d_data[d_index],
				&d_data[d_index+1],
				&d_data[d_index+2],
				&d_data[d_index+3]
			);
		}
	}
}

static inline void barycentric(
	etch_float x, 
	etch_float y,
	etch_float ax,
	etch_float ay,
	etch_float bx,
	etch_float by,
	etch_float cx,
	etch_float cy,
	etch_float *u,
	etch_float *v,
	etch_float *w
) {
	etch_float v0x = bx-ax;
	etch_float v0y = by-ay;
	etch_float v1x = cx-ax;
	etch_float v1y = cy-ay;
	etch_float v2x = x-ax;
	etch_float v2y = y-ay;
	etch_float den = v0x*v1y-v1x*v0y;
	
	*v = (v2x*v1y-v1x*v2y)/den;
    *w = (v0x*v2y-v2x*v0y)/den;
    *u = 1.0f-*v-*w;
}

static void etch_texture_triangle_top_draw(
	etch_char *d_data,
	etch_char *s_data,
	etch_float d_width,
	etch_float d_height,
	etch_float s_width,
	etch_float s_height,
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
) {
	etch_float u;
	etch_float v;
	etch_float w;
	etch_float r;
	etch_float g;
	etch_float b;
	etch_float a;
	etch_float d_x;
	etch_float d_y;
	etch_float s_x;
	etch_float s_y;
	etch_uint d_index;
	etch_uint s_index;
	etch_float d_x_max = d_width-1;
	etch_float d_y_max = d_height-1;
	etch_float slope_1 = (d_bx-d_ax)/(d_by-d_ay);
	etch_float slope_2 = (d_cx-d_ax)/(d_cy-d_ay);
	etch_float d_y_1 = MAX(d_ay,0);
	etch_float d_y_2 = MIN(d_by,d_y_max);
	etch_float d_x_1 = d_ax+slope_1*(d_y_1-d_ay);
	etch_float d_x_2 = d_ax+slope_2*(d_y_1-d_ay);
	
	d_x_1 = MAX(d_x_1,0);
	d_x_2 = MIN(d_x_2,d_x_max);
	
	for (
		d_y  = d_y_1;
		d_y <= d_y_2;
		d_y++
	) {
		for (
			d_x  = d_x_1;
			d_x <= d_x_2;
			d_x++
		) {
			barycentric(
				d_x,d_y,
				d_ax,d_ay,
				d_bx,d_by,
				d_cx,d_cy,
				&u,&v,&w
			);
			
			s_x = fmodf(u*s_ax+v*s_bx+w*s_cx,s_width);
			s_y = fmodf(u*s_ay+v*s_by+w*s_cy,s_height);
			
			d_index = (etch_uint)d_y*d_width+d_x;
			s_index = (etch_uint)s_y*s_width+s_x;
			
			d_index *= ETCH_BYTES_PER_PIXEL;
			s_index *= ETCH_BYTES_PER_PIXEL;
			
			r = (etch_float)s_data[s_index]/255;
			g = (etch_float)s_data[s_index+1]/255;
			b = (etch_float)s_data[s_index+2]/255;
			a = (etch_float)s_data[s_index+3]/255;
			
			r *= u*c_ar+v*c_br+w*c_cr;
			g *= u*c_ag+v*c_bg+w*c_cg;
			b *= u*c_ab+v*c_bb+w*c_cb;
			a *= u*c_aa+v*c_ba+w*c_ca;
			
			blend(
				(etch_char)(r*255),
				(etch_char)(g*255),
				(etch_char)(b*255),
				(etch_char)(a*255),
				d_data[d_index],
				d_data[d_index+1],
				d_data[d_index+2],
				d_data[d_index+3],
				&d_data[d_index],
				&d_data[d_index+1],
				&d_data[d_index+2],
				&d_data[d_index+3]
			);
		}
		
		d_x_1 = d_x_1+slope_1;
		d_x_2 = d_x_2+slope_2;
		d_x_1 = MAX(d_x_1,0);
		d_x_2 = MIN(d_x_2,d_x_max);
	}
}

static void etch_texture_triangle_bottom_draw(
	etch_char *d_data,
	etch_char *s_data,
	etch_float d_width,
	etch_float d_height,
	etch_float s_width,
	etch_float s_height,
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
) {
	etch_float slope_1 = (d_cx-d_ax)/(d_cy-d_ay);
	etch_float slope_2 = (d_cx-d_bx)/(d_cy-d_by);
	etch_float d_x_1 = d_cx;
	etch_float d_x_2 = d_cx;
	etch_float u;
	etch_float v;
	etch_float w;
	etch_float r;
	etch_float g;
	etch_float b;
	etch_float a;
	etch_float d_x;
	etch_float d_y;
	etch_float s_x;
	etch_float s_y;
	etch_uint d_index;
	etch_uint s_index;
	
	for (
		d_y = MIN(d_cy,s_height-1);
		d_y > MAX(d_ay,0);
		d_y--
	) {
		for (
			d_x  = MAX(d_x_1,0);
			d_x <= MIN(d_x_2,s_width-1);
			d_x++
		) {
			barycentric(
				d_x,d_y,
				d_ax,d_ay,
				d_bx,d_by,
				d_cx,d_cy,
				&u,&v,&w
			);
			
			s_x = fmodf(u*s_ax+v*s_bx+w*s_cx,s_width);
			s_y = fmodf(u*s_ay+v*s_by+w*s_cy,s_height);
			
			d_index = (etch_uint)(d_y*d_width+d_x)*ETCH_BYTES_PER_PIXEL;
			s_index = (etch_uint)(s_y*s_width+s_x)*ETCH_BYTES_PER_PIXEL;
			
			r = (etch_float)s_data[s_index]/255;
			g = (etch_float)s_data[s_index+1]/255;
			b = (etch_float)s_data[s_index+2]/255;
			a = (etch_float)s_data[s_index+3]/255;
			
			r *= u*c_ar+v*c_br+w*c_cr;
			g *= u*c_ag+v*c_bg+w*c_cg;
			b *= u*c_ab+v*c_bb+w*c_cb;
			a *= u*c_aa+v*c_ba+w*c_ca;
			
			blend(
				(etch_char)(r*255),
				(etch_char)(g*255),
				(etch_char)(b*255),
				(etch_char)(a*255),
				d_data[d_index],
				d_data[d_index+1],
				d_data[d_index+2],
				d_data[d_index+3],
				&d_data[d_index],
				&d_data[d_index+1],
				&d_data[d_index+2],
				&d_data[d_index+3]
			);
		}
		
		d_x_1 -= slope_1;
		d_x_2 -= slope_2;
	}
}

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
) {
	if (
		textures==NULL ||
		d_texture_id>=ETCH_MAX_TEXTURES ||
		textures[d_texture_id].data==NULL
	) {
		return;
	}
	
	etch_texture *d_texture = &textures[d_texture_id];
	etch_texture *s_texture = &textures[s_texture_id];
	
	etch_char *d_data    = d_texture->data;
	etch_float *d_z_data = d_texture->z_data;
	etch_float d_width   = (etch_float)d_texture->width;
	etch_float d_height  = (etch_float)d_texture->height;
	
	etch_char *s_data   = (etch_char[4]){255,255,255,255};
	etch_float s_width  = 1;
	etch_float s_height = 1;
	
	if (
		s_texture_id<ETCH_MAX_TEXTURES &&
		s_texture->data!=NULL
	) {
		s_data   = s_texture->data;
		s_width  = (etch_float)s_texture->width;
		s_height = (etch_float)s_texture->height;
	}
	
	d_ax *= d_width-1;
	d_ay *= d_height-1;
	d_bx *= d_width-1;
	d_by *= d_height-1;
	d_cx *= d_width-1;
	d_cy *= d_height-1;
	s_ax *= s_width-1;
	s_ay *= s_height-1;
	s_bx *= s_width-1;
	s_by *= s_height-1;
	s_cx *= s_width-1;
	s_cy *= s_height-1;
	
	etch_float temp_1;
	etch_float temp_2;
	etch_float temp_3;
	
	if (d_cy<d_by) {
		temp_1 = d_bx;
		temp_2 = d_by;
		temp_3 = d_bz;
		d_bx   = d_cx;
		d_by   = d_cy;
		d_bz   = d_cz;
		d_cx   = temp_1;
		d_cy   = temp_2;
		d_cz   = temp_3;
		temp_1 = s_bx;
		temp_2 = s_by;
		s_bx   = s_cx;
		s_by   = s_cy;
		s_cx   = temp_1;
		s_cy   = temp_2;
	}
	
	if (d_by<d_ay) {
		temp_1 = d_ax;
		temp_2 = d_ay;
		temp_3 = d_az;
		d_ax   = d_bx;
		d_ay   = d_by;
		d_az   = d_bz;
		d_bx   = temp_1;
		d_by   = temp_2;
		d_bz   = temp_3;
		temp_1 = s_ax;
		temp_2 = s_ay;
		s_ax   = s_bx;
		s_ay   = s_by;
		s_bx   = temp_1;
		s_by   = temp_2;
	}
	
	if (d_by==d_cy) {
		etch_texture_triangle_top_draw(
			d_data, s_data,
			d_width, d_height,
			s_width, s_height,
			d_ax, d_ay, d_az,
			d_bx, d_by, d_bz,
			d_cx, d_cy, d_cz,
			s_ax, s_ay,
			s_bx, s_by,
			s_cx, s_cy,
			c_ar, c_ag, c_ab, c_aa,
			c_br, c_bg, c_bb, c_ba,
			c_cr, c_cg, c_cb, c_ca
		);
	} else if (d_ay==d_by) {
		etch_texture_triangle_bottom_draw(
			d_data, s_data,
			d_width, d_height,
			s_width, s_height,
			d_ax, d_ay, d_az,
			d_bx, d_by, d_bz,
			d_cx, d_cy, d_cz,
			s_ax, s_ay,
			s_bx, s_by,
			s_cx, s_cy,
			c_ar, c_ag, c_ab, c_aa,
			c_br, c_bg, c_bb, c_ba,
			c_cr, c_cg, c_cb, c_ca
		);
	} else {
		
	}
}

void etch_texture_flush() {
	if (textures==NULL) {
		return;
	}
	
	for (
		etch_uint i=0;
		i<ETCH_MAX_TEXTURES;
		i++
	) {
		etch_texture_free(i);
	}
}

etch_uint etch_texture_init() {
	if (textures!=NULL) {
		return 1;
	}
	
	textures=(etch_texture *)calloc(
		ETCH_MAX_TEXTURES,
		sizeof(etch_texture)
	);
	
	if (textures==NULL) {
		return 2;
	}
	
	return 0;
}

void etch_texture_deinit() {
	etch_texture_flush();
	
	if (textures!=NULL) {
		free(textures);
		textures=NULL;
	}
}