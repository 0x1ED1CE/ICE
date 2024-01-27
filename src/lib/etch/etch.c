#include <malloc.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "etch.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define FP 255

#define DEPTH_MAX 65535

etch_texture *textures=NULL;

etch_vertex *vertexes=NULL;

const etch_sint dither_matrix[4][4]={
	{15,  135, 45,  165},
	{195, 75,  225, 105},
	{60,  180, 30,  150},
	{240, 120, 210, 90}
};

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
	texture->data   = (etch_char *)calloc(
		texture_size,
		ETCH_PIXEL_SIZE
	);
	
	if (z_enable) {
		texture->z_data=(etch_sint*)malloc(
			texture_size*sizeof(etch_sint)
		);
		
		for (etch_uint i=0; i<texture_size; i++) {
			texture->z_data[i]=DEPTH_MAX;
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
	
	etch_char *data   = texture->data;
	etch_sint *z_data = texture->z_data;
	
	if (data!=NULL) {
		memset(
			data,
			0,
			texture_size*ETCH_PIXEL_SIZE
		);
	}
	
	if (z_data!=NULL) {
		for (etch_uint i=0; i<texture_size; i++) {
			z_data[i]=DEPTH_MAX;
		}
	}
}

static inline void blend(
	etch_char c_ar,
	etch_char c_ag,
	etch_char c_ab,
	etch_char c_aa,
	etch_char c_br,
	etch_char c_bg,
	etch_char c_bb,
	etch_char c_ba,
	etch_char *c_cr,
	etch_char *c_cg,
	etch_char *c_cb,
	etch_char *c_ca
) {
	etch_sint alpha     = c_aa+1;
	etch_sint inv_alpha = 256-c_aa;
	*c_cr = (etch_char)((alpha*c_ar+inv_alpha*c_br)>>8);
	*c_cg = (etch_char)((alpha*c_ag+inv_alpha*c_bg)>>8);
	*c_cb = (etch_char)((alpha*c_ab+inv_alpha*c_bb)>>8);
	*c_ca = (etch_char)((alpha*c_aa+inv_alpha*c_ba)>>8);
}

void etch_texture_pixel_draw(
	etch_uint d_texture_id,
	etch_sint d_ax,
	etch_sint d_ay,
	etch_char c_ar,
	etch_char c_ag,
	etch_char c_ab,
	etch_char c_aa
) {
	if (
		textures==NULL ||
		d_texture_id>=ETCH_MAX_TEXTURES ||
		textures[d_texture_id].data==NULL
	) {
		return;
	}
	
	etch_texture *d_texture = &textures[d_texture_id];
	etch_char *d_data       = d_texture->data;
	etch_sint d_width       = (etch_sint)d_texture->width;
	etch_sint d_height      = (etch_sint)d_texture->height;
	
	if (
		d_ax>=d_width ||
		d_ax<0 ||
		d_ay>=d_height ||
		d_ay<0
	) {
		return;
	}
	
	etch_sint d_index = (d_ay*d_width+d_ax)*ETCH_PIXEL_SIZE;
	
	blend(
		c_ar,
		c_ag,
		c_ab,
		c_aa,
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
	
	etch_char *d_data  = d_texture->data;
	etch_sint d_width  = (etch_sint)d_texture->width;
	etch_sint d_height = (etch_sint)d_texture->height;
	
	etch_char *s_data  = (etch_char[4]){255,255,255,255};
	etch_sint s_width  = 1;
	etch_sint s_height = 1;
	
	if (
		s_texture_id<ETCH_MAX_TEXTURES &&
		s_texture->data!=NULL
	) {
		s_data   = s_texture->data;
		s_width  = (etch_sint)s_texture->width;
		s_height = (etch_sint)s_texture->height;
	}
	
	etch_sint min_x = MIN(d_ax,d_bx);
	etch_sint min_y = MIN(d_ay,d_by);
	etch_sint max_x = MAX(d_ax,d_bx);
	etch_sint max_y = MAX(d_ay,d_by);
	
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
	
	etch_sint scale_x = (s_bx-s_ax)*FP/MAX(d_bx-d_ax,1);
	etch_sint scale_y = (s_by-s_ay)*FP/MAX(d_by-d_ay,1);
	etch_sint s_x_1;
	etch_sint s_y_1 = s_ay*FP;
	etch_sint d_x;
	etch_sint d_y;
	etch_sint s_x;
	etch_sint s_y;
	etch_sint d_index;
	etch_sint s_index;
	etch_char c_br;
	etch_char c_bg;
	etch_char c_bb;
	etch_char c_ba;
	
	for (d_y=min_y; d_y<=max_y; d_y++) {
		s_y = (s_y_1/FP)%s_height;
		
		s_x_1 = s_ax*FP;
		
		for (d_x=min_x; d_x<=max_x; d_x++) {
			s_x = (s_x_1/FP)%s_width;
			
			d_index = (d_y*d_width+d_x)*ETCH_PIXEL_SIZE;
			s_index = (s_y*s_width+s_x)*ETCH_PIXEL_SIZE;
			
			c_br = (s_data[s_index]*c_ar)/255;
			c_bg = (s_data[s_index+1]*c_ag)/255;
			c_bb = (s_data[s_index+2]*c_ab)/255;
			c_ba = (s_data[s_index+3]*c_aa)/255;
			
			blend(
				c_br,
				c_bg,
				c_bb,
				c_ba,
				d_data[d_index],
				d_data[d_index+1],
				d_data[d_index+2],
				d_data[d_index+3],
				&d_data[d_index],
				&d_data[d_index+1],
				&d_data[d_index+2],
				&d_data[d_index+3]
			);
			
			s_x_1 += scale_x;
		}
		
		s_y_1 += scale_y;
	}
}

static inline etch_sint determinant(
	etch_sint ax,
	etch_sint ay,
	etch_sint bx,
	etch_sint by,
	etch_sint cx,
	etch_sint cy
) {
	return (by-ay)*(cx-ax)-(bx-ax)*(cy-ay);
}

static void scan_vector(
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
	etch_sint d_y,
	etch_sint d_x_1,
	etch_sint d_x_2,
	etch_sint *s_x_1,
	etch_sint *s_y_1,
	etch_sint *s_slope_x,
	etch_sint *s_slope_y,
	etch_sint *d_z_1,
	etch_sint *d_slope_z
) {
	etch_sint u = determinant(d_bx,d_by,d_cx,d_cy,d_x_1,d_y);
	etch_sint v = determinant(d_cx,d_cy,d_ax,d_ay,d_x_1,d_y);
	etch_sint w = determinant(d_ax,d_ay,d_bx,d_by,d_x_1,d_y);
	
	etch_sint uvw = u+v+w;
	
	u = u*FP/uvw;
	v = v*FP/uvw;
	w = w*FP/uvw;
	
	*s_x_1 = u*s_ax+v*s_bx+w*s_cx;
	*s_y_1 = u*s_ay+v*s_by+w*s_cy;
	*d_z_1 = u*d_az+v*d_bz+w*d_cz;
	
	u = determinant(d_bx,d_by,d_cx,d_cy,d_x_2,d_y);
	v = determinant(d_cx,d_cy,d_ax,d_ay,d_x_2,d_y);
	w = determinant(d_ax,d_ay,d_bx,d_by,d_x_2,d_y);
	
	uvw = u+v+w;
	
	u = u*FP/uvw;
	v = v*FP/uvw;
	w = w*FP/uvw;
	
	etch_sint s_x_2 = u*s_ax+v*s_bx+w*s_cx;
	etch_sint s_y_2 = u*s_ay+v*s_by+w*s_cy;
	etch_sint d_z_2 = u*d_az+v*d_bz+w*d_cz;
	
	etch_sint d_x_d = MAX(d_x_2-d_x_1,1);
	
	*s_slope_x = (s_x_2-*s_x_1)/d_x_d;
	*s_slope_y = (s_y_2-*s_y_1)/d_x_d;
	*d_slope_z = (d_z_2-*d_z_1)/d_x_d;
}

static void etch_texture_triangle_top_draw(
	etch_char *d_data,
	etch_char *s_data,
	etch_sint  *d_z_data,
	etch_sint d_width,
	etch_sint d_height,
	etch_sint s_width,
	etch_sint s_height,
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
) {
	if (d_cx<d_bx) {
		etch_sint temp_sint;
		etch_char temp_char;
		
		temp_sint = d_bx; d_bx = d_cx; d_cx = temp_sint;
		temp_sint = d_by; d_by = d_cy; d_cy = temp_sint;
		temp_sint = d_bz; d_bz = d_cz; d_cz = temp_sint;
		
		temp_sint = s_bx; s_bx = s_cx; s_cx = temp_sint;
		temp_sint = s_by; s_by = s_cy; s_cy = temp_sint;
		
		temp_char = c_br; c_br = c_cr; c_cr = temp_char;
		temp_char = c_bg; c_bg = c_cg; c_cg = temp_char;
		temp_char = c_bb; c_bb = c_cb; c_cb = temp_char;
		temp_char = c_ba; c_ba = c_ca; c_ca = temp_char;
	}
	
	if (determinant(d_ax,d_ay,d_bx,d_by,d_cx,d_cy)<=0) {
		return;
	}
	
	etch_sint d_x;
	etch_sint d_y;
	etch_sint d_z;
	etch_sint s_x;
	etch_sint s_y;
	etch_uint d_index;
	etch_uint s_index;
	etch_sint d_slope_1 = ((d_bx-d_ax)*FP)/(d_by-d_ay);
	etch_sint d_slope_2 = ((d_cx-d_ax)*FP)/(d_cy-d_ay);
	etch_sint d_slope_z;
	etch_sint d_y_1 = MAX(d_ay,0);
	etch_sint d_y_2 = MIN(d_by,d_height-1);
	etch_sint d_x_1 = d_ax*FP+d_slope_1*(d_y_1-d_ay);
	etch_sint d_x_2 = d_ax*FP+d_slope_2*(d_y_1-d_ay);
	etch_sint d_z_1;
	etch_sint s_x_1;
	etch_sint s_y_1;
	etch_sint s_slope_x;
	etch_sint s_slope_y;
	etch_char c_dr;
	etch_char c_dg;
	etch_char c_db;
	etch_char c_da;
	
	for (
		d_y  = d_y_1;
		d_y <= d_y_2;
		d_y ++
	) {
		d_x = MAX(d_x_1/FP,0);
		
		scan_vector(
			d_ax, d_ay, d_az,
			d_bx, d_by, d_bz,
			d_cx, d_cy, d_cz,
			s_ax, s_ay,
			s_bx, s_by,
			s_cx, s_cy,
			d_y, d_x, d_x_2/FP,
			&s_x_1, &s_y_1,
			&s_slope_x, &s_slope_y,
			&d_z_1,
			&d_slope_z
		);
		
		for (
			;
			d_x <= MIN(d_x_2/FP,d_width-1);
			d_x ++
		) {
			d_z = d_z_1/FP;
			
			d_index = (d_y*d_width+d_x);
			
			if (d_z<d_z_data[d_index]) {
				d_z_data[d_index] = d_z;
				
				s_x = (s_x_1/FP)%s_width;
				s_y = (s_y_1/FP)%s_height;
				
				d_index *= ETCH_PIXEL_SIZE;
				s_index = (s_y*s_width+s_x)*ETCH_PIXEL_SIZE;
				
				d_data[d_index]   = s_data[s_index];
				d_data[d_index+1] = s_data[s_index+1];
				d_data[d_index+2] = s_data[s_index+2];
				d_data[d_index+3] = s_data[s_index+3];
			}
			
			s_x_1 += s_slope_x;
			s_y_1 += s_slope_y;
			d_z_1 += d_slope_z;
		}
		
		d_x_1 += d_slope_1;
		d_x_2 += d_slope_2;
	}
}

static void etch_texture_triangle_bottom_draw(
	etch_char *d_data,
	etch_char *s_data,
	etch_sint  *d_z_data,
	etch_sint d_width,
	etch_sint d_height,
	etch_sint s_width,
	etch_sint s_height,
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
) {
	if (d_cx<d_ax) {
		etch_sint temp_sint;
		etch_char temp_char;
		
		temp_sint = d_ax; d_ax = d_cx; d_cx = temp_sint;
		temp_sint = d_ay; d_ay = d_cy; d_cy = temp_sint;
		temp_sint = d_az; d_az = d_cz; d_cz = temp_sint;
		
		temp_sint = s_ax; s_ax = s_cx; s_cx = temp_sint;
		temp_sint = s_ay; s_ay = s_cy; s_cy = temp_sint;
		
		temp_char = c_ar; c_ar = c_cr; c_cr = temp_char;
		temp_char = c_ag; c_ag = c_cg; c_cg = temp_char;
		temp_char = c_ab; c_ab = c_cb; c_cb = temp_char;
		temp_char = c_aa; c_aa = c_ca; c_ca = temp_char;
	}
	
	if (determinant(d_ax,d_ay,d_bx,d_by,d_cx,d_cy)<=0) {
		return;
	}
	
	etch_sint d_x;
	etch_sint d_y;
	etch_sint d_z;
	etch_sint s_x;
	etch_sint s_y;
	etch_uint d_index;
	etch_uint s_index;
	etch_sint d_slope_1 = ((d_bx-d_ax)*FP)/(d_by-d_ay);
	etch_sint d_slope_2 = ((d_bx-d_cx)*FP)/(d_by-d_cy);
	etch_sint d_slope_z;
	etch_sint d_y_1 = MIN(d_by,d_height-1);
	etch_sint d_y_2 = MAX(d_ay,0);
	etch_sint d_x_1 = d_bx*FP-d_slope_1*(d_by-d_y_1);
	etch_sint d_x_2 = d_bx*FP-d_slope_2*(d_by-d_y_1);
	etch_sint d_z_1;
	etch_sint s_x_1;
	etch_sint s_y_1;
	etch_sint s_slope_x;
	etch_sint s_slope_y;
	etch_char c_dr;
	etch_char c_dg;
	etch_char c_db;
	etch_char c_da;
	
	for (
		d_y   = d_y_1;
		d_y  >= d_y_2;
		d_y --
	) {
		d_x = MAX(d_x_1/FP,0);
		
		scan_vector(
			d_ax, d_ay, d_az,
			d_bx, d_by, d_bz,
			d_cx, d_cy, d_cz,
			s_ax, s_ay,
			s_bx, s_by,
			s_cx, s_cy,
			d_y, d_x, d_x_2/FP,
			&s_x_1, &s_y_1,
			&s_slope_x, &s_slope_y,
			&d_z_1,
			&d_slope_z
		);
		
		for (
			;
			d_x <= MIN(d_x_2/FP,d_width-1);
			d_x ++
		) {
			d_z = d_z_1/FP;
			
			d_index = (d_y*d_width+d_x);
			
			if (d_z<d_z_data[d_index]) {
				d_z_data[d_index] = d_z;
				
				s_x = (s_x_1/FP)%s_width;
				s_y = (s_y_1/FP)%s_height;
				
				d_index *= ETCH_PIXEL_SIZE;
				s_index = (s_y*s_width+s_x)*ETCH_PIXEL_SIZE;
				
				d_data[d_index]   = s_data[s_index];
				d_data[d_index+1] = s_data[s_index+1];
				d_data[d_index+2] = s_data[s_index+2];
				d_data[d_index+3] = s_data[s_index+3];
			}
			
			s_x_1 += s_slope_x;
			s_y_1 += s_slope_y;
			d_z_1 += d_slope_z;
		}
		
		d_x_1 -= d_slope_1;
		d_x_2 -= d_slope_2;
	}
}

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
) {
	if (
		determinant(d_ax,d_ay,d_bx,d_by,d_cx,d_cy)<=0 ||
		textures==NULL ||
		d_texture_id>=ETCH_MAX_TEXTURES ||
		textures[d_texture_id].z_data==NULL
	) {
		return;
	}
	
	etch_texture *d_texture = &textures[d_texture_id];
	etch_texture *s_texture = &textures[s_texture_id];
	
	etch_char *d_data   = d_texture->data;
	etch_sint *d_z_data = d_texture->z_data;
	etch_sint d_width   = (etch_sint)d_texture->width;
	etch_sint d_height  = (etch_sint)d_texture->height;
	
	etch_char *s_data  = (etch_char[4]){255,255,255,255}; //Placeholder texture
	etch_sint s_width  = 1;
	etch_sint s_height = 1;
	
	if (
		s_texture_id<ETCH_MAX_TEXTURES &&
		s_texture->data!=NULL
	) {
		s_data   = s_texture->data;
		s_width  = s_texture->width;
		s_height = s_texture->height;
	}
	
	etch_sint min_x = MIN(d_ax,d_bx);
	etch_sint min_y = MIN(d_ay,d_by);
	etch_sint min_z = MIN(d_az,d_bz);
	etch_sint max_x = MAX(d_ax,d_bx);
	etch_sint max_y = MAX(d_ay,d_by);
	etch_sint max_z = MAX(d_az,d_bz);
	
	min_x = MIN(min_x,d_cx);
	min_y = MIN(min_y,d_cy);
	min_z = MIN(min_z,d_cz);
	max_x = MAX(max_x,d_cx);
	max_y = MAX(max_y,d_cy);
	max_z = MAX(max_z,d_cz);
	
	if ( //Return if triangle is outside of visibility
		min_x>=d_width ||
		max_x<0 ||
		min_y>=d_height ||
		max_y<0 ||
		min_z>=DEPTH_MAX ||
		max_z<0
	) {
		return;
	}
	
	etch_sint temp_sint;
	etch_char temp_char;
	
	if (d_ay>d_cy) {
		temp_sint = d_ax; d_ax = d_cx; d_cx = temp_sint;
		temp_sint = d_ay; d_ay = d_cy; d_cy = temp_sint;
		temp_sint = d_az; d_az = d_cz; d_cz = temp_sint;
		
		temp_sint = s_ax; s_ax = s_cx; s_cx = temp_sint;
		temp_sint = s_ay; s_ay = s_cy; s_cy = temp_sint;
		
		temp_char = c_ar; c_ar = c_cr; c_cr = temp_char;
		temp_char = c_ag; c_ag = c_cg; c_cg = temp_char;
		temp_char = c_ab; c_ab = c_cb; c_cb = temp_char;
		temp_char = c_aa; c_aa = c_ca; c_ca = temp_char;
	}
	
	if (d_ay>d_by) {
		temp_sint = d_ax; d_ax = d_bx; d_bx = temp_sint;
		temp_sint = d_ay; d_ay = d_by; d_by = temp_sint;
		temp_sint = d_az; d_az = d_bz; d_bz = temp_sint;
		
		temp_sint = s_ax; s_ax = s_bx; s_bx = temp_sint;
		temp_sint = s_ay; s_ay = s_by; s_by = temp_sint;
		
		temp_char = c_ar; c_ar = c_br; c_br = temp_char;
		temp_char = c_ag; c_ag = c_bg; c_bg = temp_char;
		temp_char = c_ab; c_ab = c_bb; c_bb = temp_char;
		temp_char = c_aa; c_aa = c_ba; c_ba = temp_char;
	}
	
	if (d_by>d_cy) {
		temp_sint = d_bx; d_bx = d_cx; d_cx = temp_sint;
		temp_sint = d_by; d_by = d_cy; d_cy = temp_sint;
		temp_sint = d_bz; d_bz = d_cz; d_cz = temp_sint;
		
		temp_sint = s_bx; s_bx = s_cx; s_cx = temp_sint;
		temp_sint = s_by; s_by = s_cy; s_cy = temp_sint;
		
		temp_char = c_br; c_br = c_cr; c_cr = temp_char;
		temp_char = c_bg; c_bg = c_cg; c_cg = temp_char;
		temp_char = c_bb; c_bb = c_cb; c_cb = temp_char;
		temp_char = c_ba; c_ba = c_ca; c_ca = temp_char;
	}
	
	if (d_by==d_cy) {
		etch_texture_triangle_top_draw(
			d_data, s_data,
			d_z_data,
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
			d_z_data,
			d_width, d_height,
			s_width, s_height,
			d_ax, d_ay, d_az,
			d_cx, d_cy, d_cz,
			d_bx, d_by, d_bz,
			s_ax, s_ay, 
			s_cx, s_cy,
			s_bx, s_by,
			c_ar, c_ag, c_ab, c_aa,
			c_cr, c_cg, c_cb, c_ca,
			c_br, c_bg, c_bb, c_ba
		);
	} else {
		temp_sint = ((d_by-d_ay)*FP)/(d_cy-d_ay);
		
		etch_sint d_dx = d_ax+((temp_sint*(d_cx-d_ax))/FP);
		etch_sint d_dy = d_by;
		etch_sint d_dz = d_az+((temp_sint*(d_cz-d_az))/FP);
		
		etch_sint s_dx = s_ax+((temp_sint*(s_cx-s_ax))/FP);
		etch_sint s_dy = s_ay+((temp_sint*(s_cy-s_ay))/FP);
		
		etch_sint c_dr = c_ar+((temp_sint*(c_cr-c_ar))/FP);
		etch_sint c_dg = c_ag+((temp_sint*(c_cg-c_ag))/FP);
		etch_sint c_db = c_ab+((temp_sint*(c_cb-c_ab))/FP);
		etch_sint c_da = c_aa+((temp_sint*(c_ca-c_aa))/FP);
		
		etch_texture_triangle_top_draw(
			d_data, s_data,
			d_z_data,
			d_width, d_height,
			s_width, s_height,
			d_ax, d_ay, d_az,
			d_bx, d_by, d_bz,
			d_dx, d_dy, d_dz,
			s_ax, s_ay,
			s_bx, s_by,
			s_dx, s_dy,
			c_ar, c_ag, c_ab, c_aa,
			c_br, c_bg, c_bb, c_ba,
			c_dr, c_dg, c_db, c_da
		);
		
		etch_texture_triangle_bottom_draw(
			d_data, s_data,
			d_z_data,
			d_width, d_height,
			s_width, s_height,
			d_bx, d_by, d_bz,
			d_cx, d_cy, d_cz,
			d_dx, d_dy, d_dz,
			s_bx, s_by,
			s_cx, s_cy,
			s_dx, s_dy,
			c_br, c_bg, c_bb, c_ba,
			c_cr, c_cg, c_cb, c_ca,
			c_dr, c_dg, c_db, c_da
		);
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

etch_uint etch_vertex_new(
	etch_uint size
) {
	if (vertexes==NULL) {
		return 0;
	}
	
	etch_uint vertex_id = 0;
	etch_vertex *vertex = NULL;
	
	for (etch_uint i=0; i<ETCH_MAX_VERTEXES; i++) {
		if (vertexes[i].data==NULL) {
			vertex_id = i;
			vertex    = &vertexes[i];
			
			break;
		}
	}
	
	if (vertex==NULL) {
		return 0;
	}
	
	vertex->size = size;
	vertex->data = (etch_word *)calloc(
		size,
		sizeof(etch_word)
	);
	
	return vertex_id;
}

void etch_vertex_free(
	etch_uint vertex_id
) {
	if (
		vertexes==NULL ||
		vertex_id>=ETCH_MAX_VERTEXES
	) {
		return;
	}
	
	etch_vertex *vertex=&vertexes[vertex_id];
	
	if (vertex->data!=NULL) {
		free(vertex->data);
		vertex->data=NULL;
	}
	
	vertex->size=0;
}

etch_uint etch_vertex_size_get(
	etch_uint vertex_id
) {
	if (
		vertexes==NULL ||
		vertex_id>=ETCH_MAX_VERTEXES
	) {
		return 0;
	}
	
	etch_vertex *vertex=&vertexes[vertex_id];
	
	return vertex->size;
}

void etch_vertex_set_uint(
	etch_uint vertex_id,
	etch_uint index,
	etch_uint value
) {
	if (
		vertexes==NULL ||
		vertex_id>=ETCH_MAX_VERTEXES ||
		vertexes[vertex_id].data==NULL
	) {
		return;
	}
	
	etch_vertex *vertex = &vertexes[vertex_id];
	etch_uint size      = vertex->size;
	etch_word *data     = vertex->data;
	
	data[index%size].uint = value;
}

etch_uint etch_vertex_get_uint(
	etch_uint vertex_id,
	etch_uint index
) {
	if (
		vertexes==NULL ||
		vertex_id>=ETCH_MAX_VERTEXES ||
		vertexes[vertex_id].data==NULL
	) {
		return 0;
	}
	
	etch_vertex *vertex = &vertexes[vertex_id];
	etch_uint size      = vertex->size;
	etch_word *data     = vertex->data;
	
	return data[index%size].uint;
}

void etch_vertex_set_sint(
	etch_uint vertex_id,
	etch_uint index,
	etch_sint value
) {
	if (
		vertexes==NULL ||
		vertex_id>=ETCH_MAX_VERTEXES ||
		vertexes[vertex_id].data==NULL
	) {
		return;
	}
	
	etch_vertex *vertex = &vertexes[vertex_id];
	etch_uint size      = vertex->size;
	etch_word *data     = vertex->data;
	
	data[index%size].sint = value;
}

etch_sint etch_vertex_get_sint(
	etch_uint vertex_id,
	etch_uint index
) {
	if (
		vertexes==NULL ||
		vertex_id>=ETCH_MAX_VERTEXES ||
		vertexes[vertex_id].data==NULL
	) {
		return 0;
	}
	
	etch_vertex *vertex = &vertexes[vertex_id];
	etch_uint size      = vertex->size;
	etch_word *data     = vertex->data;
	
	return data[index%size].sint;
}

void etch_vertex_set_real(
	etch_uint vertex_id,
	etch_uint index,
	etch_real value
) {
	if (
		vertexes==NULL ||
		vertex_id>=ETCH_MAX_VERTEXES ||
		vertexes[vertex_id].data==NULL
	) {
		return;
	}
	
	etch_vertex *vertex = &vertexes[vertex_id];
	etch_uint size      = vertex->size;
	etch_word *data     = vertex->data;
	
	data[index%size].real = value;
}

etch_real etch_vertex_get_real(
	etch_uint vertex_id,
	etch_uint index
) {
	if (
		vertexes==NULL ||
		vertex_id>=ETCH_MAX_VERTEXES ||
		vertexes[vertex_id].data==NULL
	) {
		return 0;
	}
	
	etch_vertex *vertex = &vertexes[vertex_id];
	etch_uint size      = vertex->size;
	etch_word *data     = vertex->data;
	
	return data[index%size].real;
}

void etch_vertex_texture_draw(
	etch_uint d_texture_id,
	etch_uint s_texture_id,
	etch_real render_matrix[4][4],
	etch_uint position_vertex_id,
	etch_uint texture_vertex_id,
	etch_uint face_vertex_id
) {
	if (
		textures==NULL ||
		d_texture_id>=ETCH_MAX_TEXTURES ||
		textures[d_texture_id].z_data==NULL ||
		vertexes==NULL ||
		position_vertex_id>=ETCH_MAX_VERTEXES ||
		texture_vertex_id>=ETCH_MAX_VERTEXES ||
		face_vertex_id>=ETCH_MAX_VERTEXES ||
		vertexes[position_vertex_id].data==NULL ||
		vertexes[texture_vertex_id].data==NULL ||
		vertexes[face_vertex_id].data==NULL
	) {
		return;
	}
	
	etch_texture *d_texture = &textures[d_texture_id];
	etch_texture *s_texture = &textures[s_texture_id];
	
	etch_real d_width    = (etch_real)d_texture->width-1;
	etch_real d_height   = (etch_real)d_texture->height-1;
	etch_real d_width_h  = d_width/2;
	etch_real d_height_h = d_height/2;
	etch_real s_width    = 1;
	etch_real s_height   = 1;
	
	if (
		s_texture_id<ETCH_MAX_TEXTURES &&
		s_texture->data!=NULL
	) {
		s_width  = (etch_real)s_texture->width;
		s_height = (etch_real)s_texture->height;
	}
	
	etch_vertex *position_vertex = &vertexes[position_vertex_id];
	etch_vertex *texture_vertex  = &vertexes[texture_vertex_id];
	etch_vertex *face_vertex     = &vertexes[face_vertex_id];
	
	etch_uint position_vertexes = position_vertex->size;
	etch_uint texture_vertexes  = texture_vertex->size;
	etch_uint face_vertexes     = face_vertex->size;
	
	etch_word *position_data = position_vertex->data;
	etch_word *texture_data  = texture_vertex->data;
	etch_word *face_data     = face_vertex->data;
	
	etch_uint f_i;
	etch_uint v_i;
	
	etch_real p_ax; //Positions
	etch_real p_ay;
	etch_real p_az;
	etch_real p_bx;
	etch_real p_by;
	etch_real p_bz;
	etch_real p_cx;
	etch_real p_cy;
	etch_real p_cz;
	
	etch_real v_ax; //Transformed positions
	etch_real v_ay;
	etch_real v_az;
	etch_real v_aw;
	etch_real v_bx;
	etch_real v_by;
	etch_real v_bz;
	etch_real v_bw;
	etch_real v_cx;
	etch_real v_cy;
	etch_real v_cz;
	etch_real v_cw;
	
	etch_sint t_ax; //Texture coordinates
	etch_sint t_ay;
	etch_sint t_bx;
	etch_sint t_by;
	etch_sint t_cx;
	etch_sint t_cy;
	
	etch_sint d_ax; //Final vector
	etch_sint d_ay;
	etch_sint d_az;
	etch_sint d_bx;
	etch_sint d_by;
	etch_sint d_bz;
	etch_sint d_cx;
	etch_sint d_cy;
	etch_sint d_cz;
	
	etch_char c_ar = 255; //Color
	etch_char c_ag = 255;
	etch_char c_ab = 255;
	etch_char c_aa = 255;
	etch_char c_br = 255;
	etch_char c_bg = 255;
	etch_char c_bb = 255;
	etch_char c_ba = 255;
	etch_char c_cr = 255;
	etch_char c_cg = 255;
	etch_char c_cb = 255;
	etch_char c_ca = 255;
	
	for (f_i=0; f_i<face_vertexes; f_i+=6) {
		v_i  = face_data[f_i].uint*3;
		p_ax = position_data[v_i].real;
		p_ay = position_data[v_i+1].real;
		p_az = position_data[v_i+2].real;
		
		v_i  = face_data[f_i+1].uint*2;
		t_ax = (etch_sint)(texture_data[v_i].real*s_width);
		t_ay = (etch_sint)(texture_data[v_i+1].real*s_height);
		
		v_i  = face_data[f_i+2].uint*3;
		p_bx = position_data[v_i].real;
		p_by = position_data[v_i+1].real;
		p_bz = position_data[v_i+2].real;
		
		v_i  = face_data[f_i+3].uint*2;
		t_bx = (etch_sint)(texture_data[v_i].real*s_width);
		t_by = (etch_sint)(texture_data[v_i+1].real*s_height);
		
		v_i  = face_data[f_i+4].uint*3;
		p_cx = position_data[v_i].real;
		p_cy = position_data[v_i+1].real;
		p_cz = position_data[v_i+2].real;
		
		v_i  = face_data[f_i+5].uint*2;
		t_cx = (etch_sint)(texture_data[v_i].real*s_width);
		t_cy = (etch_sint)(texture_data[v_i+1].real*s_height);
		
		v_ax = (
			p_ax*render_matrix[0][0]+
			p_ay*render_matrix[0][1]+
			p_az*render_matrix[0][2]+
			render_matrix[0][3]
		);
		v_ay = (
			p_ax*render_matrix[1][0]+
			p_ay*render_matrix[1][1]+
			p_az*render_matrix[1][2]+
			render_matrix[1][3]
		);
		v_az = (
			p_ax*render_matrix[2][0]+
			p_ay*render_matrix[2][1]+
			p_az*render_matrix[2][2]+
			render_matrix[2][3]
		);
		v_aw = (
			p_ax*render_matrix[3][0]+
			p_ay*render_matrix[3][1]+
			p_az*render_matrix[3][2]+
			render_matrix[3][3]
		);
		
		v_bx = (
			p_bx*render_matrix[0][0]+
			p_by*render_matrix[0][1]+
			p_bz*render_matrix[0][2]+
			render_matrix[0][3]
		);
		v_by = (
			p_bx*render_matrix[1][0]+
			p_by*render_matrix[1][1]+
			p_bz*render_matrix[1][2]+
			render_matrix[1][3]
		);
		v_bz = (
			p_bx*render_matrix[2][0]+
			p_by*render_matrix[2][1]+
			p_bz*render_matrix[2][2]+
			render_matrix[2][3]
		);
		v_bw = (
			p_bx*render_matrix[3][0]+
			p_by*render_matrix[3][1]+
			p_bz*render_matrix[3][2]+
			render_matrix[3][3]
		);
		
		v_cx = (
			p_cx*render_matrix[0][0]+
			p_cy*render_matrix[0][1]+
			p_cz*render_matrix[0][2]+
			render_matrix[0][3]
		);
		v_cy = (
			p_cx*render_matrix[1][0]+
			p_cy*render_matrix[1][1]+
			p_cz*render_matrix[1][2]+
			render_matrix[1][3]
		);
		v_cz = (
			p_cx*render_matrix[2][0]+
			p_cy*render_matrix[2][1]+
			p_cz*render_matrix[2][2]+
			render_matrix[2][3]
		);
		v_cw = (
			p_cx*render_matrix[3][0]+
			p_cy*render_matrix[3][1]+
			p_cz*render_matrix[3][2]+
			render_matrix[3][3]
		);
		
		d_ax = (etch_sint)((v_ax/v_aw+1)*d_width_h);
		d_ay = (etch_sint)(d_height-(v_ay/v_aw+1)*d_height_h);
		d_az = (etch_sint)(v_az*FP);
		
		d_bx = (etch_sint)((v_bx/v_bw+1)*d_width_h);
		d_by = (etch_sint)(d_height-(v_by/v_bw+1)*d_height_h);
		d_bz = (etch_sint)(v_bz*FP);
		
		d_cx = (etch_sint)((v_cx/v_cw+1)*d_width_h);
		d_cy = (etch_sint)(d_height-(v_cy/v_cw+1)*d_height_h);
		d_cz = (etch_sint)(v_cz*FP);
		
		if (v_aw>0 && v_bw>0 && v_cw>0) {
			etch_texture_triangle_draw(
				d_texture_id,
				s_texture_id,
				d_ax, d_ay, d_az,
				d_bx, d_by, d_bz,
				d_cx, d_cy, d_cz,
				t_ax, t_ay,
				t_bx, t_by,
				t_cx, t_cy,
				c_ar, c_ag, c_ab, c_aa,
				c_br, c_bg, c_bb, c_ba,
				c_cr, c_cg, c_cb, c_ca
			);
		}
	}
}

void etch_vertex_flush() {
	if (vertexes==NULL) {
		return;
	}
	
	for (
		etch_uint i=0;
		i<ETCH_MAX_VERTEXES;
		i++
	) {
		etch_vertex_free(i);
	}
}

etch_uint etch_vertex_init() {
	if (vertexes!=NULL) {
		return 1;
	}
	
	vertexes=(etch_vertex *)calloc(
		ETCH_MAX_VERTEXES,
		sizeof(etch_vertex)
	);
	
	if (vertexes==NULL) {
		return 2;
	}
	
	return 0;
}

void etch_vertex_deinit() {
	etch_vertex_flush();
	
	if (vertexes!=NULL) {
		free(vertexes);
		vertexes=NULL;
	}
}