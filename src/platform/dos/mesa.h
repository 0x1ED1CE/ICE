#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/dmesa.h>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/stb_image.h"

#define MAX_TEXTURES 256
#define MAX_ARRAYS   256

typedef struct {
	ice_uint type;
	ice_uint size;
	GLfloat  *data;
} ice_video_array;

static DMesaVisual  visual;
static DMesaContext context;
static DMesaBuffer  buffers[2];

static unsigned int current_buffer;

static GLuint *textures;

static ice_video_array *arrays;

ice_uint ice_video_texture_load(
	ice_uint file_id
) {
	char filename[32];
	sprintf(
		filename,
		"%u.png",
		(unsigned int)file_id
	);
	
	int width;
	int height;
	int bpp;
	
	unsigned char *data=stbi_load(
		filename,
		&width,
		&height,
		&bpp,
		STBI_rgb_alpha
	);
	
	if (data==NULL) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to load texture: %u",
			file_id
		);
		ice_log(msg);
		
		return 0;
	}
	
	for (ice_uint i=1; i<MAX_TEXTURES; i++) {
		if (textures[i]==0) {
			glGenTextures(1,&textures[i]);
			glBindTexture(GL_TEXTURE_2D,textures[i]);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_S,
				GL_REPEAT
			);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_T,
				GL_REPEAT
			);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MIN_FILTER,
				GL_LINEAR
			);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MAG_FILTER,
				GL_LINEAR
			);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				(GLsizei)width, //Size must be power of 2 for GL1.1
				(GLsizei)height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				data
			);
			glBindTexture(GL_TEXTURE_2D,0);
			
			stbi_image_free(data);
			
			return i;
		}
	}
	
	stbi_image_free(data);
	
	{
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to create GL texture: %u",
			(unsigned int)glGetError()
		);
		ice_log(msg);
	}
	
	return 0;
}

void ice_video_texture_delete(
	ice_uint texture_id
) {
	if (
		textures==NULL ||
		texture_id>MAX_TEXTURES ||
		textures[texture_id]==0
	) {
		return;
	}
	
	glDeleteTextures(
		1,
		&textures[texture_id]
	);
	
	textures[texture_id]=0;
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
	glEnable(GL_BLEND);
	glBlendFunc(
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA
	);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(
		GL_TEXTURE_ENV,
		GL_COMBINE_RGB,
		GL_MODULATE
	);
	glTexEnvi(
		GL_TEXTURE_ENV,
		GL_COMBINE_ALPHA,
		GL_MODULATE
	);
	glBindTexture(
		GL_TEXTURE_2D,
		textures[s_texture_id]
	);
	glBegin(GL_QUADS);
	glColor4f(
		(GLfloat)c_ar,
		(GLfloat)c_ag,
		(GLfloat)c_ab,
		(GLfloat)c_aa
	);
	glTexCoord2f(
		(GLfloat)s_ax,
		(GLfloat)s_ay
	);
	glVertex3f(
		(GLfloat)d_ax*2.0-1.0,
		1.0-(GLfloat)d_ay*2.0,
		0.0
	);
	glTexCoord2f(
		(GLfloat)s_ax,
		(GLfloat)s_by
	);
	glVertex3f(
		(GLfloat)d_ax*2.0-1.0,
		1.0-(GLfloat)d_by*2.0,
		0.0
	);
	glTexCoord2f(
		(GLfloat)s_bx,
		(GLfloat)s_by
	);
	glVertex3f(
		(GLfloat)d_bx*2.0-1.0,
		1.0-(GLfloat)d_by*2.0,
		0.0
	);
	glTexCoord2f(
		(GLfloat)s_bx,
		(GLfloat)s_ay
	);
	glVertex3f(
		(GLfloat)d_bx*2.0-1.0,
		1.0-(GLfloat)d_ay*2.0,
		0.0
	);
	glEnd();
	glFlush();
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
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
	if (textures==NULL) {
		return;
	}
	
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(
		GL_TEXTURE_ENV,
		GL_COMBINE_RGB,
		GL_MODULATE
	);
	glTexEnvi(
		GL_TEXTURE_ENV,
		GL_COMBINE_ALPHA,
		GL_MODULATE
	);
	glBindTexture(
		GL_TEXTURE_2D,
		textures[s_texture_id]
	);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(
		(GLfloat)s_ax,
		(GLfloat)s_ay
	);
	glColor4f(
		(GLfloat)c_ar,
		(GLfloat)c_ag,
		(GLfloat)c_ab,
		(GLfloat)c_aa
	);
	glVertex3f(
		(GLfloat)d_ax*2.0-1.0, //Convert to cartesian coordinates
		1.0-(GLfloat)d_ay*2.0,
		(GLfloat)d_az
	);
	glTexCoord2f(
		(GLfloat)s_bx,
		(GLfloat)s_by
	);
	glColor4f(
		(GLfloat)c_br,
		(GLfloat)c_bg,
		(GLfloat)c_bb,
		(GLfloat)c_ba
	);
	glVertex3f(
		(GLfloat)d_bx*2.0-1.0,
		1.0-(GLfloat)d_by*2.0,
		(GLfloat)d_bz
	);
	glTexCoord2f(
		(GLfloat)s_cx,
		(GLfloat)s_cy
	);
	glColor4f(
		(GLfloat)c_cr,
		(GLfloat)c_cg,
		(GLfloat)c_cb,
		(GLfloat)c_ca
	);
	glVertex3f(
		(GLfloat)d_cx*2.0-1.0,
		1.0-(GLfloat)d_cy*2.0,
		(GLfloat)d_cz
	);
	glEnd();
	glFlush();
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

ice_uint ice_video_array_new(
	ice_uint type,
	ice_uint size
) {
	if (
		arrays==NULL ||
		type==0 ||
		size==0
	) {
		return 0;
	}
	
	for (ice_uint i=0; i<MAX_ARRAYS; i++) {
		if (arrays[i].data==NULL) {
			arrays[i].type = type;
			arrays[i].size = size;
			arrays[i].data = calloc(
				size,
				sizeof(GLfloat)
			);
			
			return i;
		}
	}
	
	return 0;
}

void ice_video_array_delete(
	ice_uint array_id
) {
	if (
		arrays==NULL ||
		array_id>MAX_ARRAYS ||
		arrays[array_id].data==NULL
	) {
		return;
	}
	
	ice_video_array *array = &arrays[array_id];
	
	free(array->data);
	
	array->data = NULL;
	array->type = 0;
	array->size = 0;
}

ice_uint ice_video_array_size_get(
	ice_uint array_id
) {
	if (
		arrays==NULL ||
		array_id>MAX_ARRAYS ||
		arrays[array_id].data==NULL
	) {
		return 0;
	}
	
	return arrays[array_id].size;
}

void ice_video_array_set(
	ice_uint array_id,
	ice_uint index,
	ice_real value
) {
	if (
		arrays==NULL ||
		array_id>MAX_ARRAYS ||
		arrays[array_id].data==NULL
	) {
		return;
	}
	
	ice_video_array *array = &arrays[array_id];
	
	if (index<array->size) {
		array->data[index]=(GLfloat)value;
	}
}

ice_real ice_video_array_get(
	ice_uint array_id,
	ice_uint index
) {
	if (
		arrays==NULL ||
		array_id>MAX_ARRAYS ||
		arrays[array_id].data==NULL
	) {
		return 0;
	}
	
	ice_video_array *array = &arrays[array_id];
	
	if (index<array->size) {
		return (ice_real)array->data[index];
	}
	
	return 0;
}

void ice_video_model_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_uint projection_array_id,
	ice_uint model_array_id
) {
	if (
		arrays==NULL ||
		projection_array_id>MAX_ARRAYS ||
		model_array_id>MAX_ARRAYS
	) {
		return;
	}
	
	ice_video_array *projection_array = &arrays[projection_array_id];
	ice_video_array *model_array      = &arrays[model_array_id];
	
	if (
		projection_array->data==NULL ||
		projection_array->type!=ICE_ARRAY_TYPE_MATRIX_16 ||
		projection_array->size%16!=0 ||
		model_array->data==NULL ||
		model_array->type!=ICE_ARRAY_TYPE_MODEL ||
		model_array->size%2!=0
	) {
		return;
	}
	
	ice_uint position_array_id = (ice_uint)model_array->data[0];
	ice_uint texture_array_id  = (ice_uint)model_array->data[1];
	
	if (
		position_array_id>MAX_ARRAYS ||
		texture_array_id>MAX_ARRAYS
	) {
		return;
	}
	
	ice_video_array *position_array = &arrays[position_array_id];
	ice_video_array *texture_array  = &arrays[texture_array_id];
	
	if (
		position_array->data==NULL ||
		position_array->type!=ICE_ARRAY_TYPE_POSITION_3 ||
		position_array->size%3!=0 ||
		texture_array->data==NULL ||
		texture_array->type!=ICE_ARRAY_TYPE_TEXTURE_2 ||
		texture_array->size%2!=0
	) {
		return;
	}
	
	GLfloat *position_data = position_array->data;
	GLfloat *texture_data  = texture_array->data;
	
	ice_uint p = 0;
	ice_uint t = 0;
	
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	/*
	glTexEnvi(
		GL_TEXTURE_ENV,
		GL_COMBINE_RGB,
		GL_MODULATE
	);
	glTexEnvi(
		GL_TEXTURE_ENV,
		GL_COMBINE_ALPHA,
		GL_MODULATE
	);
	*/
	glBindTexture(
		GL_TEXTURE_2D,
		textures[s_texture_id]
	);
	glLoadMatrixf(projection_array->data);
	glBegin(GL_TRIANGLES);
	
	while (p<position_array->size) {
		glTexCoord2fv(texture_data+t); //TODO: Cache into displaylist
		glVertex3fv(position_data+p);
		glTexCoord2fv(texture_data+t+2);
		glVertex3fv(position_data+p+3);
		glTexCoord2fv(texture_data+t+4);
		glVertex3fv(position_data+p+6);
		
		p += 9;
		t += 6;
	}
	
	glEnd();
	glFlush();
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void ice_video_texture_flush() {
	if (textures==NULL) {
		return;
	}
	
	for (ice_uint i=1; i<MAX_TEXTURES; i++) {
		ice_video_texture_delete(i);
	}
}

void ice_video_array_flush() {
	if (arrays==NULL) {
		return;
	}
	
	for (ice_uint i=1; i<MAX_ARRAYS; i++) {
		ice_video_array_delete(i);
	}
}

void ice_video_buffer() {
	if (current_buffer==0) {
		current_buffer=1;
	} else {
		current_buffer=0;
	}
	
	DMesaSwapBuffers(buffers[current_buffer]);
	
	glClearColor(0,0,0,0);
	glClear(
		GL_COLOR_BUFFER_BIT|
		GL_DEPTH_BUFFER_BIT
	);
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
		1,   // double-buffered
		1,   // RGB mode
		8,   // requested bits/alpha
		16,  // requested bits/depth
		0,   // requested bits/stencil
		0    // requested bits/accum
	);
	
	context = DMesaCreateContext(
		visual,
		NULL
	);
	
	buffers[0] = DMesaCreateBuffer(
		visual,
		0,   // X pos
		0,   // Y pos
		640, // X res
		480  // Y res
	);
	
	buffers[1] = DMesaCreateBuffer(
		visual,
		0,   // X pos
		0,   // Y pos
		640, // X res
		480  // Y res
	);
	
	DMesaMakeCurrent(
		context,
		buffers[0]
	);
	
	textures=(GLuint *)calloc(
		MAX_TEXTURES,
		sizeof(GLuint)
	);
	
	arrays=(ice_video_array *)calloc(
		MAX_ARRAYS,
		sizeof(ice_video_array)
	);
	
	return 0;
}

void ice_video_deinit() {	
	ice_video_texture_flush();
	ice_video_array_flush();
	
	if (textures!=NULL) {
		free(textures);
		textures=NULL;
	}
	
	if (arrays!=NULL) {
		free(arrays);
		arrays=NULL;
	}
	
	DMesaDestroyBuffer(buffers[0]);
	DMesaDestroyBuffer(buffers[1]);
	DMesaDestroyContext(context);
	DMesaDestroyVisual(visual);
}
