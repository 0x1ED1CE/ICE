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
	texture->data   = (etch_pixel*)calloc(
		texture_size,
		sizeof(etch_pixel)
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
	
	etch_pixel *data  = texture->data;
	etch_sint *z_data = texture->z_data;
	
	if (data!=NULL) {
		memset(
			data,
			0,
			texture_size*sizeof(etch_pixel)
		);
	}
	
	if (z_data!=NULL) {
		for (etch_uint i=0; i<texture_size; i++) {
			z_data[i]=DEPTH_MAX;
		}
	}
}

static inline void blend(
	etch_pixel a,
	etch_pixel b,
	etch_pixel *c
) {
	etch_sint alpha     = a.a+1;
	etch_sint inv_alpha = 256-a.a;
	c->r = (etch_char)((alpha*a.r+inv_alpha*b.r)>>8);
	c->g = (etch_char)((alpha*a.g+inv_alpha*b.g)>>8);
	c->b = (etch_char)((alpha*a.b+inv_alpha*b.b)>>8);
	c->a = (etch_char)((alpha*a.a+inv_alpha*b.a)>>8);
}

void etch_texture_pixel_draw(
	etch_uint d_texture_id,
	etch_sint_vector2 d_a,
	etch_pixel c_a
) {
	if (
		textures==NULL ||
		d_texture_id>=ETCH_MAX_TEXTURES ||
		textures[d_texture_id].data==NULL
	) {
		return;
	}
	
	etch_texture *d_texture = &textures[d_texture_id];
	etch_pixel *d_data      = d_texture->data;
	etch_sint d_width       = (etch_sint)d_texture->width;
	etch_sint d_height      = (etch_sint)d_texture->height;
	
	if (
		d_a.x>=d_width ||
		d_a.x<0 ||
		d_a.y>=d_height ||
		d_a.y<0
	) {
		return;
	}
	
	etch_sint d_index = d_a.y*d_width+d_a.x;
	
	blend(
		c_a,
		d_data[d_index],
		&d_data[d_index]
	);
}

void etch_texture_rectangle_draw(
	etch_uint d_texture_id,
	etch_uint s_texture_id,
	etch_sint_vector2 d_a,
	etch_sint_vector2 d_b,
	etch_sint_vector2 s_a,
	etch_sint_vector2 s_b,
	etch_pixel c_a
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
	
	etch_pixel *d_data = d_texture->data;
	etch_sint d_width  = (etch_sint)d_texture->width;
	etch_sint d_height = (etch_sint)d_texture->height;
	
	etch_pixel *s_data = &(etch_pixel){255,255,255,255};
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
	
	etch_sint min_x = MIN(d_a.x,d_b.x);
	etch_sint min_y = MIN(d_a.y,d_b.y);
	etch_sint max_x = MAX(d_a.x,d_b.x);
	etch_sint max_y = MAX(d_a.y,d_b.y);
	
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
	
	etch_sint scale_x = (s_b.x-s_a.x)*FP/MAX(d_b.x-d_a.x,1);
	etch_sint scale_y = (s_b.y-s_a.y)*FP/MAX(d_b.y-d_a.y,1);
	etch_sint s_x_1;
	etch_sint s_y_1 = s_a.y*FP;
	etch_sint d_x;
	etch_sint d_y;
	etch_sint s_x;
	etch_sint s_y;
	etch_sint d_index;
	etch_sint s_index;
	etch_pixel c_b;
	
	for (d_y=min_y; d_y<=max_y; d_y++) {
		s_y = (s_y_1/FP)%s_height;
		
		s_x_1 = s_a.x*FP;
		
		for (d_x=min_x; d_x<=max_x; d_x++) {
			s_x = (s_x_1/FP)%s_width;
			
			d_index = d_y*d_width+d_x;
			s_index = s_y*s_width+s_x;
			
			c_b=s_data[s_index];
			
			c_b.r = (c_b.r*c_a.r)/255;
			c_b.g = (c_b.g*c_a.g)/255;
			c_b.b = (c_b.b*c_a.b)/255;
			c_b.a = (c_b.a*c_a.a)/255;
			
			blend(
				c_b,
				d_data[d_index],
				&d_data[d_index]
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

void etch_texture_triangle_draw_old(
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
) {
	if (
		textures==NULL ||
		d_texture_id>=ETCH_MAX_TEXTURES ||
		//textures[d_texture_id].data==NULL ||
		textures[d_texture_id].z_data==NULL ||
		determinant(d_a.x,d_a.y,d_b.x,d_b.y,d_c.x,d_c.y)<=0
	) {
		return;
	}
	
	etch_texture *d_texture = &textures[d_texture_id];
	etch_texture *s_texture = &textures[s_texture_id];
	
	etch_pixel *d_data  = d_texture->data;
	etch_sint *d_z_data = d_texture->z_data;
	etch_sint d_width   = (etch_sint)d_texture->width;
	etch_sint d_height  = (etch_sint)d_texture->height;
	
	etch_pixel *s_data = &(etch_pixel){255,255,255,255}; //Placeholder texture
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
	
	etch_sint min_x = MIN(d_a.x,d_b.x);
	etch_sint min_y = MIN(d_a.y,d_b.y);
	etch_sint min_z = MIN(d_a.z,d_b.z);
	etch_sint max_x = MAX(d_a.x,d_b.x);
	etch_sint max_y = MAX(d_a.y,d_b.y);
	etch_sint max_z = MAX(d_a.z,d_b.z);
	
	min_x = MIN(min_x,d_c.x);
	min_y = MIN(min_y,d_c.y);
	min_z = MIN(min_z,d_c.z);
	max_x = MAX(max_x,d_c.x);
	max_y = MAX(max_y,d_c.y);
	max_z = MAX(max_z,d_c.z);
	
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
	
	min_x = MAX(min_x,0); //Clip within screen space
	min_y = MAX(min_y,0);
	max_x = MIN(max_x,d_width-1);
	max_y = MIN(max_y,d_height-1);
	
	etch_sint d_x;
	etch_sint d_y;
	etch_sint d_z;
	etch_sint s_x;
	etch_sint s_y;
	etch_sint d_index;
	etch_sint s_index;
	etch_sint u;
	etch_sint v;
	etch_sint w;
	etch_sint uvw;
	etch_pixel c_d;
	
	for (d_y=min_y; d_y<=max_y; d_y++) {
		for (d_x=min_x; d_x<=max_x; d_x++) {
			u = determinant(d_b.x,d_b.y,d_c.x,d_c.y,d_x,d_y);
			v = determinant(d_c.x,d_c.y,d_a.x,d_a.y,d_x,d_y);
			w = determinant(d_a.x,d_a.y,d_b.x,d_b.y,d_x,d_y);
			
			if (u>=0 && v>=0 && w>=0) { //Edge test
				uvw = u+v+w;
				
				u = u*FP/uvw;
				v = v*FP/uvw;
				w = w*FP/uvw;
				
				d_z=(u*d_a.z+v*d_b.z+w*d_c.z)/FP;
				
				d_index = d_y*d_width+d_x;
				
				if (d_z<d_z_data[d_index]) { //Depth test
					s_x = ((u*s_a.x+v*s_b.x+w*s_c.x)/FP)%s_width;
					s_y = ((u*s_a.y+v*s_b.y+w*s_c.y)/FP)%s_height;
					
					s_index = s_y*s_width+s_x;
					
					c_d = s_data[s_index];
					
					c_d.a = (c_d.a*(u*c_a.a/255+v*c_b.a/255+w*c_c.a/255))/FP;
					
					if (c_d.a>=dither_matrix[d_x%4][d_y%4]) { //Alpha test
						c_d.r = (c_d.r*(u*c_a.r/255+v*c_b.r/255+w*c_c.r/255))/FP;
						c_d.g = (c_d.g*(u*c_a.g/255+v*c_b.g/255+w*c_c.g/255))/FP;
						c_d.b = (c_d.b*(u*c_a.b/255+v*c_b.b/255+w*c_c.b/255))/FP;
						
						d_data[d_index]   = c_d;
						d_z_data[d_index] = d_z;
					}
				}
			}
		}
	}
}

static void texture_vector(
	etch_sint_vector3 d_a,
	etch_sint_vector3 d_b,
	etch_sint_vector3 d_c,
	etch_sint_vector2 s_a,
	etch_sint_vector2 s_b,
	etch_sint_vector2 s_c,
	etch_sint d_y,
	etch_sint d_x_1,
	etch_sint d_x_2,
	etch_sint *s_x_1,
	etch_sint *s_y_1,
	etch_sint *s_slope_x,
	etch_sint *s_slope_y
) {
	etch_sint u = determinant(d_b.x,d_b.y,d_c.x,d_c.y,d_x_1,d_y);
	etch_sint v = determinant(d_c.x,d_c.y,d_a.x,d_a.y,d_x_1,d_y);
	etch_sint w = determinant(d_a.x,d_a.y,d_b.x,d_b.y,d_x_1,d_y);
	
	etch_sint uvw = MAX(u+v+w,1);
	
	u = u*FP/uvw;
	v = v*FP/uvw;
	w = w*FP/uvw;
	
	*s_x_1 = u*s_a.x+v*s_b.x+w*s_c.x;
	*s_y_1 = u*s_a.y+v*s_b.y+w*s_c.y;
	
	u = determinant(d_b.x,d_b.y,d_c.x,d_c.y,d_x_2,d_y);
	v = determinant(d_c.x,d_c.y,d_a.x,d_a.y,d_x_2,d_y);
	w = determinant(d_a.x,d_a.y,d_b.x,d_b.y,d_x_2,d_y);
	
	uvw = MAX(u+v+w,1);
	
	u = u*FP/uvw;
	v = v*FP/uvw;
	w = w*FP/uvw;
	
	etch_sint s_x_2 = u*s_a.x+v*s_b.x+w*s_c.x;
	etch_sint s_y_2 = u*s_a.y+v*s_b.y+w*s_c.y;
	
	etch_sint d_x_d = MAX(d_x_2-d_x_1,1);
	
	*s_slope_x = (s_x_2-*s_x_1)/d_x_d;
	*s_slope_y = (s_y_2-*s_y_1)/d_x_d;
}

static void etch_texture_triangle_top_draw(
	etch_pixel *d_data,
	etch_pixel *s_data,
	etch_sint  *d_z_data,
	etch_sint d_width,
	etch_sint d_height,
	etch_sint s_width,
	etch_sint s_height,
	etch_sint_vector3 d_a,
	etch_sint_vector3 d_b,
	etch_sint_vector3 d_c,
	etch_sint_vector2 s_a,
	etch_sint_vector2 s_b,
	etch_sint_vector2 s_c,
	etch_pixel c_a,
	etch_pixel c_b,
	etch_pixel c_c
) {
	if (d_c.x<d_b.x) {
		etch_sint_vector3 temp_vector3 = d_b; d_b = d_c; d_c = temp_vector3;
		etch_sint_vector2 temp_vector2 = s_b; s_b = s_c; s_c = temp_vector2;
		etch_pixel temp_pixel = c_b; c_b = c_c; c_c = temp_pixel;
	}
	
	etch_sint d_x;
	etch_sint d_y;
	etch_sint d_z=1;
	etch_sint s_x;
	etch_sint s_y;
	etch_uint d_index;
	etch_uint s_index;
	etch_sint d_slope_1 = ((d_b.x-d_a.x)*FP)/(d_b.y-d_a.y);
	etch_sint d_slope_2 = ((d_c.x-d_a.x)*FP)/(d_c.y-d_a.y);
	etch_sint d_y_1 = MAX(d_a.y,0);
	etch_sint d_y_2 = MIN(d_b.y,d_height-1);
	etch_sint d_x_1 = d_a.x*FP+d_slope_1*(d_y_1-d_a.y);
	etch_sint d_x_2 = d_a.x*FP+d_slope_2*(d_y_1-d_a.y);
	etch_sint s_x_1;
	etch_sint s_y_1;
	etch_sint s_slope_x;
	etch_sint s_slope_y;
	etch_pixel c_d;
	
	for (
		d_y  = d_y_1;
		d_y <= d_y_2;
		d_y ++
	) {
		d_x = MAX(d_x_1/FP,0);
		
		texture_vector(
			d_a, d_b, d_c,
			s_a, s_b, s_c,
			d_y, d_x, d_x_2/FP,
			&s_x_1, &s_y_1,
			&s_slope_x, &s_slope_y
		);
		
		for (
			;
			d_x <= MIN(d_x_2/FP,d_width-1);
			d_x ++
		) {
			s_x = (s_x_1/FP)%s_width;
			s_y = (s_y_1/FP)%s_height;
			
			d_index = d_y*d_width+d_x;
			s_index = s_y*s_width+s_x;
			
			d_data[d_index] = s_data[s_index];
			
			s_x_1 += s_slope_x;
			s_y_1 += s_slope_y;
		}
		
		d_x_1 += d_slope_1;
		d_x_2 += d_slope_2;
	}
}

static void etch_texture_triangle_bottom_draw(
	etch_pixel *d_data,
	etch_pixel *s_data,
	etch_sint  *d_z_data,
	etch_sint d_width,
	etch_sint d_height,
	etch_sint s_width,
	etch_sint s_height,
	etch_sint_vector3 d_a,
	etch_sint_vector3 d_b,
	etch_sint_vector3 d_c,
	etch_sint_vector2 s_a,
	etch_sint_vector2 s_b,
	etch_sint_vector2 s_c,
	etch_pixel c_a,
	etch_pixel c_b,
	etch_pixel c_c
) {
	if (d_c.x<d_a.x) {
		etch_sint_vector3 temp_vector3 = d_a; d_a = d_c; d_c = temp_vector3;
		etch_sint_vector2 temp_vector2 = s_a; s_a = s_c; s_c = temp_vector2;
		etch_pixel temp_pixel = c_a; c_a = c_c; c_c = temp_pixel;
	}
	
	etch_sint d_x;
	etch_sint d_y;
	etch_sint d_z=1;
	etch_sint s_x;
	etch_sint s_y;
	etch_uint d_index;
	etch_uint s_index;
	etch_sint d_slope_1 = ((d_b.x-d_a.x)*FP)/(d_b.y-d_a.y);
	etch_sint d_slope_2 = ((d_b.x-d_c.x)*FP)/(d_b.y-d_c.y);
	etch_sint d_y_1 = MIN(d_b.y,d_height-1);
	etch_sint d_y_2 = MAX(d_a.y,0);
	etch_sint d_x_1 = d_b.x*FP-d_slope_1*(d_b.y-d_y_1);
	etch_sint d_x_2 = d_b.x*FP-d_slope_2*(d_b.y-d_y_1);
	etch_sint s_x_1;
	etch_sint s_y_1;
	etch_sint s_slope_x;
	etch_sint s_slope_y;
	etch_pixel c_d;
	
	for (
		d_y   = d_y_1;
		d_y  >= d_y_2;
		d_y --
	) {
		d_x = MAX(d_x_1/FP,0);
		
		texture_vector(
			d_a, d_b, d_c,
			s_a, s_b, s_c,
			d_y, d_x, d_x_2/FP,
			&s_x_1, &s_y_1,
			&s_slope_x, &s_slope_y
		);
		
		for (
			;
			d_x <= MIN(d_x_2/FP,d_width-1);
			d_x ++
		) {
			//d_z=(u*d_a.z+v*d_b.z+w*d_c.z)/FP;
				
			d_index = d_y*d_width+d_x;
			/*
			if (d_z<d_z_data[d_index]) { //Depth test
				//s_x = ((u*s_a.x+v*s_b.x+w*s_c.x)/FP)%s_width;
				//s_y = ((u*s_a.y+v*s_b.y+w*s_c.y)/FP)%s_height;
				
				s_x = (s_x_1/FP)%s_width;
				s_y = (s_y_1/FP)%s_height;
				
				s_index = s_y*s_width+s_x;
				
				c_d = s_data[s_index];
				
				//c_d.a = (c_d.a*(u*c_a.a/255+v*c_b.a/255+w*c_c.a/255))/FP;
				
				if (c_d.a>=dither_matrix[d_x%4][d_y%4]) { //Alpha test
					//c_d.r = (c_d.r*(u*c_a.r/255+v*c_b.r/255+w*c_c.r/255))/FP;
					//c_d.g = (c_d.g*(u*c_a.g/255+v*c_b.g/255+w*c_c.g/255))/FP;
					//c_d.b = (c_d.b*(u*c_a.b/255+v*c_b.b/255+w*c_c.b/255))/FP;
					
					//d_data[d_index]   = c_d;
					//d_z_data[d_index] = d_z;
				}
				
				d_data[d_index]   = c_d;
			}
			*/
			
			s_x = (s_x_1/FP)%s_width;
			s_y = (s_y_1/FP)%s_height;
			
			s_index = s_y*s_width+s_x;
			
			d_data[d_index] = s_data[s_index];
			
			s_x_1 += s_slope_x;
			s_y_1 += s_slope_y;
		}
		
		d_x_1 -= d_slope_1;
		d_x_2 -= d_slope_2;
	}
}

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
) {
	if (
		textures==NULL ||
		d_texture_id>=ETCH_MAX_TEXTURES ||
		textures[d_texture_id].z_data==NULL ||
		determinant(d_a.x,d_a.y,d_b.x,d_b.y,d_c.x,d_c.y)<=0
	) {
		return;
	}
	
	etch_texture *d_texture = &textures[d_texture_id];
	etch_texture *s_texture = &textures[s_texture_id];
	
	etch_pixel *d_data  = d_texture->data;
	etch_sint *d_z_data = d_texture->z_data;
	etch_sint d_width   = (etch_sint)d_texture->width;
	etch_sint d_height  = (etch_sint)d_texture->height;
	
	etch_pixel *s_data = &(etch_pixel){255,255,255,255}; //Placeholder texture
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
	
	etch_sint_vector3 temp_sint_vector3;
	etch_sint_vector2 temp_sint_vector2;
	etch_pixel temp_pixel;
	
	if (d_a.y>d_c.y) {
		temp_sint_vector3 = d_a; d_a = d_c; d_c = temp_sint_vector3;
		temp_sint_vector2 = s_a; s_a = s_c; s_c = temp_sint_vector2;
		temp_pixel = c_a; c_a = c_c; c_c = temp_pixel;
	}
	
	if (d_a.y>d_b.y) {
		temp_sint_vector3 = d_a; d_a = d_b; d_b = temp_sint_vector3;
		temp_sint_vector2 = s_a; s_a = s_b; s_b = temp_sint_vector2;
		temp_pixel = c_a; c_a = c_b; c_b = temp_pixel;
	}
	
	if (d_b.y>d_c.y) {
		temp_sint_vector3 = d_b; d_b = d_c; d_c = temp_sint_vector3;
		temp_sint_vector2 = s_b; s_b = s_c; s_c = temp_sint_vector2;
		temp_pixel = c_b; c_b = c_c; c_c = temp_pixel;
	}
	
	if (d_b.y==d_c.y) {
		etch_texture_triangle_top_draw(
			d_data, s_data,
			d_z_data,
			d_width, d_height,
			s_width, s_height,
			d_a, d_b, d_c,
			s_a, s_b, s_c,
			c_a, c_b, c_c
		);
	} else if (d_a.y==d_b.y) {
		etch_texture_triangle_bottom_draw(
			d_data, s_data,
			d_z_data,
			d_width, d_height,
			s_width, s_height,
			d_a, d_c, d_b,
			s_a, s_c, s_b,
			c_a, c_c, c_b
		);
	} else {
		etch_sint temp_sint = ((d_b.y-d_a.y)*FP)/(d_c.y-d_a.y);
		
		etch_sint_vector3 d_d = (etch_sint_vector3){
			d_a.x+((temp_sint*(d_c.x-d_a.x))/FP),
			d_b.y,
			d_a.z+((temp_sint*(d_c.z-d_a.z))/FP)
		};
		
		etch_sint_vector2 s_d = (etch_sint_vector2){
			s_a.x+((temp_sint*(s_c.x-s_a.x))/FP),
			s_a.y+((temp_sint*(s_c.y-s_a.y))/FP)
		};
		
		etch_pixel c_d = (etch_pixel){
			c_a.r+((temp_sint*(c_c.r-c_a.r))/FP),
			c_a.g+((temp_sint*(c_c.g-c_a.g))/FP),
			c_a.b+((temp_sint*(c_c.b-c_a.b))/FP),
			c_a.a+((temp_sint*(c_c.a-c_a.a))/FP)
		};
		
		etch_texture_triangle_top_draw(
			d_data, s_data,
			d_z_data,
			d_width, d_height,
			s_width, s_height,
			d_a, d_b, d_d,
			s_a, s_b, s_d,
			c_a, c_b, c_d
		);
		etch_texture_triangle_bottom_draw(
			d_data, s_data,
			d_z_data,
			d_width, d_height,
			s_width, s_height,
			d_b, d_c, d_d,
			s_b, s_c, s_d,
			c_b, c_c, c_d
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
	
	etch_real_vector3 p_a; //Positions
	etch_real_vector3 p_b;
	etch_real_vector3 p_c;
	
	etch_real_vector3 v_a; //Transformed positions
	etch_real_vector3 v_b;
	etch_real_vector3 v_c;
	
	etch_sint_vector2 t_a; //Texture coordinates
	etch_sint_vector2 t_b;
	etch_sint_vector2 t_c;
	
	etch_sint_vector3 d_a; //Final vector
	etch_sint_vector3 d_b;
	etch_sint_vector3 d_c;
	
	etch_pixel c_a = (etch_pixel){255,255,255,255};
	etch_pixel c_b = (etch_pixel){255,255,255,255};
	etch_pixel c_c = (etch_pixel){255,255,255,255};
	
	for (f_i=0; f_i<face_vertexes; f_i+=6) {
		v_i   = face_data[f_i].uint*3;
		p_a.x = position_data[v_i].real;
		p_a.y = position_data[v_i+1].real;
		p_a.z = position_data[v_i+2].real;
		
		v_i   = face_data[f_i+1].uint*2;
		t_a.x = (etch_sint)(texture_data[v_i].real*s_width);
		t_a.y = (etch_sint)(texture_data[v_i+1].real*s_height);
		
		v_i   = face_data[f_i+2].uint*3;
		p_b.x = position_data[v_i].real;
		p_b.y = position_data[v_i+1].real;
		p_b.z = position_data[v_i+2].real;
		
		v_i   = face_data[f_i+3].uint*2;
		t_b.x = (etch_sint)(texture_data[v_i].real*s_width);
		t_b.y = (etch_sint)(texture_data[v_i+1].real*s_height);
		
		v_i   = face_data[f_i+4].uint*3;
		p_c.x = position_data[v_i].real;
		p_c.y = position_data[v_i+1].real;
		p_c.z = position_data[v_i+2].real;
		
		v_i   = face_data[f_i+5].uint*2;
		t_c.x = (etch_sint)(texture_data[v_i].real*s_width);
		t_c.y = (etch_sint)(texture_data[v_i+1].real*s_height);
		
		v_a.x = (
			render_matrix[0][3]+
			p_a.x*render_matrix[0][0]+
			p_a.y*render_matrix[0][1]+
			p_a.z*render_matrix[0][2]
		);
		v_a.y = (
			render_matrix[1][3]+
			p_a.x*render_matrix[1][0]+
			p_a.y*render_matrix[1][1]+
			p_a.z*render_matrix[1][2]
		);
		v_a.z = (
			render_matrix[2][3]+
			p_a.x*render_matrix[2][0]+
			p_a.y*render_matrix[2][1]+
			p_a.z*render_matrix[2][2]
		);
		
		v_b.x = (
			render_matrix[0][3]+
			p_b.x*render_matrix[0][0]+
			p_b.y*render_matrix[0][1]+
			p_b.z*render_matrix[0][2]
		);
		v_b.y = (
			render_matrix[1][3]+
			p_b.x*render_matrix[1][0]+
			p_b.y*render_matrix[1][1]+
			p_b.z*render_matrix[1][2]
		);
		v_b.z = (
			render_matrix[2][3]+
			p_b.x*render_matrix[2][0]+
			p_b.y*render_matrix[2][1]+
			p_b.z*render_matrix[2][2]
		);
		
		v_c.x = (
			render_matrix[0][3]+
			p_c.x*render_matrix[0][0]+
			p_c.y*render_matrix[0][1]+
			p_c.z*render_matrix[0][2]
		);
		v_c.y = (
			render_matrix[1][3]+
			p_c.x*render_matrix[1][0]+
			p_c.y*render_matrix[1][1]+
			p_c.z*render_matrix[1][2]
		);
		v_c.z = (
			render_matrix[2][3]+
			p_c.x*render_matrix[2][0]+
			p_c.y*render_matrix[2][1]+
			p_c.z*render_matrix[2][2]
		);
		
		d_a.x = (etch_sint)((v_a.x/v_a.z+1)*d_width_h);
		d_a.y = (etch_sint)(d_height-(v_a.y/v_a.z+1)*d_height_h);
		d_a.z = (etch_sint)(v_a.z);
		
		d_b.x = (etch_sint)((v_b.x/v_b.z+1)*d_width_h);
		d_b.y = (etch_sint)(d_height-(v_b.y/v_b.z+1)*d_height_h);
		d_b.z = (etch_sint)(v_b.z);
		
		d_c.x = (etch_sint)((v_c.x/v_c.z+1)*d_width_h);
		d_c.y = (etch_sint)(d_height-(v_c.y/v_c.z+1)*d_height_h);
		d_c.z = (etch_sint)(v_c.z);
		
		if (d_a.z>0 && d_b.z>0 && d_c.z>0) {
			etch_texture_triangle_draw(
				d_texture_id,
				s_texture_id,
				d_a, d_b, d_c,
				t_a, t_b, t_c,
				c_a, c_b, c_c
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