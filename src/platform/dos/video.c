#include <stdint.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/dmesa.h>

#include "ice.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/stb_image.h"

#ifndef PL_MPEG_IMPLEMENTATION
#define PL_MPEG_IMPLEMENTATION
#include "lib/plmpeg/plmpeg.h"
#endif

#include "lib/mat/mat.h"

#define MAX_TEXTURES 256
#define MAX_STREAMS  8
#define MAX_ARRAYS   256

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

typedef struct {
	ice_uint type;
	ice_uint size;
	GLfloat *data;
	GLuint   list;
} ice_video_array;

typedef struct {
	plm_t   *plm;
	uint8_t *buffer;
	ice_uint texture_id;
} ice_video_stream;

static DMesaVisual  visual;
static DMesaContext context;
static DMesaBuffer  buffers[2];

static unsigned int current_buffer;

static GLuint *textures;

static ice_video_stream *streams;

static ice_video_array *arrays;

ice_uint ice_video_width_get() {
	return 640;
}

ice_uint ice_video_height_get() {
	return 480;
}

ice_uint ice_video_texture_new(
	ice_uint width,
	ice_uint height,
	GLenum format
) {
	if (
		textures==NULL ||
		width==0 ||
		height==0
	) {
		ice_log((ice_char *)"Invalid texture parameters!");
		
		return 0;
	}
	
	for (ice_uint i=1; i<MAX_TEXTURES; i++) {
		if (textures[i]==0) {
			glGenTextures(1,&textures[i]);
			glBindTexture(
				GL_TEXTURE_2D,
				textures[i]
			);
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
				format,
				(GLsizei)width, //Size must be power of 2 for GL1.1
				(GLsizei)height,
				0,
				format,
				GL_UNSIGNED_BYTE,
				NULL
			);
			glBindTexture(GL_TEXTURE_2D,0);
			
			GLuint gl_error=glGetError();
			
			if (gl_error) {
				ice_char msg[128];
				
				sprintf(
					(char *)msg,
					"Failed to create OpenGL texture: %u",
					(unsigned int)gl_error
				);
				
				ice_log(msg);
				
				ice_video_texture_delete(i);
				
				return 0;
			}
			
			return i;
		}
	}
	
	ice_log((ice_char *)"Cannot exceed max textures limit");
	
	return 0;
}

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
			(unsigned int)file_id
		);
		ice_log(msg);
		
		return 0;
	}
	
	ice_uint texture_id=ice_video_texture_new(
		(ice_uint)width,
		(ice_uint)height,
		GL_RGBA
	);
	
	if (texture_id==0) {
		stbi_image_free(data);
		
		return 0;
	}
	
	glBindTexture(
		GL_TEXTURE_2D,
		textures[texture_id]
	);
	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		(GLsizei)width,
		(GLsizei)height,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data
	);
	glBindTexture(GL_TEXTURE_2D,0);
	
	stbi_image_free(data);
	
	return texture_id;
}

void ice_video_texture_delete(
	ice_uint texture_id
) {
	//texture_id&=0xFFFF;
	
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
	texture_id&=0xFFFF;
	
	if (
		textures==NULL ||
		texture_id>MAX_TEXTURES ||
		textures[texture_id]==0
	) {
		return 0;
	}
	
	GLint width;
	
	glGetTexLevelParameteriv(
		GL_TEXTURE_2D,
		0,
		GL_TEXTURE_WIDTH,
		&width
	);
	
	return (ice_uint)width;
}

ice_uint ice_video_texture_height_get(
	ice_uint texture_id
) {
	texture_id&=0xFFFF;
	
	if (
		textures==NULL ||
		texture_id>MAX_TEXTURES ||
		textures[texture_id]==0
	) {
		return 0;
	}
	
	GLint height;
	
	glGetTexLevelParameteriv(
		GL_TEXTURE_2D,
		0,
		GL_TEXTURE_WIDTH,
		&height
	);
	
	return (ice_uint)height;
}

void ice_video_texture_rectangle_draw(
	ice_uint texture_id,
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
	texture_id&=0xFFFF;
	
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
		textures[texture_id]
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
	ice_uint texture_id,
	ice_real d_ax,
	ice_real d_ay,
	ice_real d_bx,
	ice_real d_by,
	ice_real d_cx,
	ice_real d_cy,
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
	texture_id&=0xFFFF;
	
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
		textures[texture_id]
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
		(GLfloat)d_ax*2.0-1.0, //Convert to cartesian
		1.0-(GLfloat)d_ay*2.0,
		0.0
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
		0.0
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
		0.0
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
		ice_log((ice_char *)"Cannot create invalid video array object");
		
		return 0;
	}
	
	for (ice_uint i=0; i<MAX_ARRAYS; i++) {
		if (arrays[i].data==NULL) {
			arrays[i].data = calloc(
				size,
				sizeof(GLfloat)
			);
			
			if (arrays[i].data==NULL) {
				ice_log((ice_char *)"Failed to allocate video array object");
				
				return 0;
			}
			
			arrays[i].type = type;
			arrays[i].size = size;
			
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
	
	if (array->list) {
		glDeleteLists(array->list,1);
	}
	
	array->data = NULL;
	array->list = 0;
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

ice_uint ice_video_model_load(
	ice_uint file_id
) {
	char filename[32];
	sprintf(
		filename,
		"%u.mat",
		(unsigned int)file_id
	);
	
	mat_list vertexes;
	mat_list textures;
	mat_list faces;
	
	mat_read_attribute(
		&vertexes,
		filename,
		MAT_ATTRIBUTE_VERT,
		0
	);
	
	mat_read_attribute(
		&textures,
		filename,
		MAT_ATTRIBUTE_TEXT,
		0
	);
	
	if (
		vertexes.data==NULL ||
		textures.data==NULL
	) {
		mat_free(&vertexes);
		mat_free(&textures);
		
		return 0;
	};
	
	mat_uint entry = 0;
	mat_read_attribute(
		&faces,
		filename,
		MAT_ATTRIBUTE_FACE,
		entry
	);
	
	ice_uint model_id = ice_video_array_new(
		ICE_VIDEO_ARRAY_TYPE_MODEL_3,
		(ice_uint)mat_count_attributes(
			filename,
			MAT_ATTRIBUTE_FACE
		)*3
	);
	
	for (ice_uint m=0; faces.data!=NULL; m+=3) {
		ice_uint vertex_id = ice_video_array_new(
			ICE_VIDEO_ARRAY_TYPE_POSITION_3,
			(ice_uint)faces.size
		);
		
		ice_uint texture_id = ice_video_array_new(
			ICE_VIDEO_ARRAY_TYPE_TEXTURE_2,
			(ice_uint)faces.size/9*6
		);
		
		for (ice_uint i=0; i<faces.size; i+=3) {
			ice_uint vi = (ice_uint)faces.data[i]*3;
			ice_uint ti = (ice_uint)faces.data[i+2]*2;
			
			for (ice_uint j=0; j<3; j++) {
				ice_video_array_set(
					vertex_id,
					i+j,
					(ice_real)vertexes.data[vi+j]
				);
			}
			
			for (ice_uint j=0; j<2; j++) {
				ice_video_array_set(
					texture_id,
					i/3*2+j,
					(ice_real)textures.data[ti+j]
				);
			}
		}
		
		ice_video_array_set(model_id,m,(ice_real)vertex_id);
		ice_video_array_set(model_id,m+1,(ice_real)texture_id);
		
		mat_free(&faces);
		
		mat_read_attribute(
			&faces,
			filename,
			MAT_ATTRIBUTE_FACE,
			++entry
		);
	};
	
	mat_free(&vertexes);
	mat_free(&textures);
	
	return model_id;
}

ice_video_array *ice_video_array_fetch(
	ice_uint array_id
) {
	if (
		arrays==NULL ||
		array_id>MAX_ARRAYS ||
		arrays[array_id].data==NULL
	) {
		return NULL;
	}
	
	return &arrays[array_id];
}

void ice_video_model_draw(
	ice_uint model_array_id,
	ice_uint projection_array_id,
	ice_real color_r,
	ice_real color_g,
	ice_real color_b,
	ice_real color_a
) {
	ice_video_array *projection_array=ice_video_array_fetch(
		projection_array_id
	);
	ice_video_array *model_array=ice_video_array_fetch(
		model_array_id
	);
	
	if (
		projection_array==NULL ||
		projection_array->type!=ICE_VIDEO_ARRAY_TYPE_MATRIX_16 ||
		model_array==NULL ||
		model_array->type!=ICE_VIDEO_ARRAY_TYPE_MODEL_3
	) {
		return;
	}
	
	//glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_EQUAL,1.0);
	glLoadMatrixf(projection_array->data);
	
	if (model_array->list==0) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		
		for (ice_uint m=0; m<model_array->size; m+=3) {
			ice_video_array *position_array=ice_video_array_fetch(
				(ice_uint)model_array->data[m]
			);
			ice_video_array *texture_array=ice_video_array_fetch(
				(ice_uint)model_array->data[m+1]
			);
			glBindTexture(
				GL_TEXTURE_2D,
				textures[(ice_uint)(model_array->data[m+2])&0xFFFF]
			);
			
			/*
			if (
				position_array==NULL ||
				position_array->type!=ICE_VIDEO_ARRAY_TYPE_POSITION_3 ||
				texture_array==NULL ||
				texture_array->type!=ICE_VIDEO_ARRAY_TYPE_TEXTURE_2
			) {
				return;
			}
			*/
			
			GLfloat *position_data = position_array->data;
			GLfloat *texture_data  = texture_array->data;
			
			//glDepthMask(GL_FALSE);
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
			glVertexPointer(
				3,
				GL_FLOAT,
				0,
				position_data
			);
			glTexCoordPointer(
				2,
				GL_FLOAT,
				0,
				texture_data
			);
			glColor4f(
				(GLfloat)color_r,
				(GLfloat)color_g,
				(GLfloat)color_b,
				(GLfloat)color_a
			);
			glDrawArrays(
				GL_TRIANGLES,
				0,
				position_array->size/3
			);
		}
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	} else {
		glCallList(model_array->list);
	}
	
	glFlush();
	glBindTexture(GL_TEXTURE_2D,0);
	//glDepthMask(GL_TRUE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	//glDisable(GL_BLEND);
}

ice_uint ice_video_stream_load(
	ice_uint file_id
) {
	char filename[32];
	sprintf(
		filename,
		"%u.mpg",
		(unsigned int)file_id
	);
	
	plm_t *plm = plm_create_with_filename(filename);
	
	if (plm==NULL) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Failed to load video stream: %u",
			(unsigned int)file_id
		);
		ice_log(msg);
		
		return 0;
	}
	
	plm_set_audio_enabled(plm,FALSE);
	plm_set_video_enabled(plm,FALSE);
	
	ice_uint width  = (ice_uint)plm_get_width(plm);
	ice_uint height = (ice_uint)plm_get_height(plm);
	
	uint8_t *buffer = malloc(
		width*height*3
	);
	
	if (buffer==NULL) {
		ice_log((ice_char *)"Failed to allocate video stream buffer");
		
		plm_destroy(plm);
		
		return 0;
	}
	
	ice_uint texture_id = ice_video_texture_new(
		width,
		height,
		GL_RGB
	);
	
	if (texture_id==0) {
		plm_destroy(plm);
		free(buffer);
		
		return 0;
	}
	
	for (ice_uint i=0; i<MAX_STREAMS; i++) {
		if (streams[i].plm==NULL) {
			streams[i].plm        = plm;
			streams[i].buffer     = buffer;
			streams[i].texture_id = texture_id;
			
			return (i<<16)|streams[i].texture_id;
		}
	}
	
	ice_log((ice_char *)"Exceeded texture streams limit");
	
	return 0;
}

void ice_video_stream_delete(
	ice_uint stream_id
) {
	stream_id>>=16;
	
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].plm==NULL
	) {
		return;
	}
	
	ice_video_stream *stream = &streams[stream_id];
	
	plm_destroy(stream->plm);
	free(stream->buffer);
	ice_video_texture_delete(stream->texture_id);
	
	stream->plm        = NULL;
	stream->buffer     = NULL;
	stream->texture_id = 0;
}

ice_real ice_video_stream_length_get(
	ice_uint stream_id
) {
	stream_id>>=16;
	
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].plm==NULL
	) {
		return 0;
	}
	
	ice_video_stream *stream = &streams[stream_id];
	
	return (ice_real)plm_get_duration(stream->plm);
}

ice_real ice_video_stream_width_get(
	ice_uint stream_id
) {
	stream_id>>=16;
	
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].plm==NULL
	) {
		return 0;
	}
	
	ice_video_stream *stream = &streams[stream_id];
	
	return (ice_real)plm_get_width(stream->plm);
}

ice_real ice_video_stream_height_get(
	ice_uint stream_id
) {
	stream_id>>=16;
	
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].plm==NULL
	) {
		return 0;
	}
	
	ice_video_stream *stream = &streams[stream_id];
	
	return (ice_real)plm_get_height(stream->plm);
}

ice_real ice_video_stream_position_get(
	ice_uint stream_id
) {
	stream_id>>=16;
	
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].plm==NULL
	) {
		return 0;
	}
	
	ice_video_stream *stream = &streams[stream_id];
	
	return (ice_real)plm_get_time(stream->plm);
}

void ice_video_stream_position_set(
	ice_uint stream_id,
	ice_real position
) {
	stream_id>>=16;
	
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].plm==NULL
	) {
		return;
	}
	
	ice_video_stream *stream = &streams[stream_id];
	
	plm_seek(
		stream->plm,
		(double)position,
		1
	);
}

ice_uint ice_video_stream_state_get(
	ice_uint stream_id
) {
	stream_id>>=16;
	
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].plm==NULL
	) {
		return ICE_VIDEO_STATE_PAUSED;
	}
	
	ice_video_stream *stream = &streams[stream_id];
	
	if (plm_get_loop(stream->plm)) {
		return ICE_VIDEO_STATE_LOOPING;
	} else if(plm_get_video_enabled(stream->plm)) {
		return ICE_VIDEO_STATE_PLAYING;
	}
	
	return ICE_VIDEO_STATE_PAUSED;
}

void ice_video_stream_state_set(
	ice_uint stream_id,
	ice_uint state
) {
	stream_id>>=16;
	
	if (
		streams==NULL ||
		stream_id>=MAX_STREAMS ||
		streams[stream_id].plm==NULL
	) {
		return;
	}
	
	ice_video_stream *stream = &streams[stream_id];
	
	switch(state) {
		case ICE_VIDEO_STATE_PAUSED:
			plm_set_video_enabled(
				stream->plm,
				FALSE
			);
			plm_set_loop(
				stream->plm,
				FALSE
			);
			
			break;
		case ICE_VIDEO_STATE_PLAYING:
			plm_set_video_enabled(
				stream->plm,
				TRUE
			);
			plm_set_loop(
				stream->plm,
				FALSE
			);
			
			break;
		case ICE_VIDEO_STATE_LOOPING:
			plm_set_video_enabled(
				stream->plm,
				TRUE
			);
			plm_set_loop(
				stream->plm, 
				TRUE
			);
	}
}

void ice_video_depth_set(
	ice_uint state
) {
	if (!state) {
		glDisable(GL_DEPTH_TEST);
	} else {
		glEnable(GL_DEPTH_TEST);
	}
}

ice_uint ice_video_depth_get() {
	return (ice_uint)glIsEnabled(GL_DEPTH_TEST);
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

void ice_video_stream_flush() {
	if (streams==NULL) {
		return;
	}
	
	for (ice_uint i=0; i<MAX_STREAMS; i++) {
		ice_video_stream_delete(i<<16);
	}
}

void ice_video_buffer(ice_real tick) {
	current_buffer=~current_buffer&0x01;
	
	DMesaSwapBuffers(buffers[current_buffer]);
	
	//Clear screen
	glClearColor(0,0,0,0);
	glClear(
		GL_COLOR_BUFFER_BIT|
		GL_DEPTH_BUFFER_BIT
	);
	
	//Update texture streams
	for (ice_uint i=0; i<MAX_STREAMS; i++) {
		ice_video_stream *stream=&streams[i];
		
		if (
			stream->plm!=NULL && 
			plm_get_video_enabled(stream->plm)
		) {
			int width     = plm_get_width(stream->plm);
			int height    = plm_get_height(stream->plm);
			ice_real time = plm_get_time(stream->plm);
			
			plm_frame_t *frame;
			
			do {
				frame=plm_decode_video(stream->plm);
			} while (
				frame!=NULL &&
				(ice_real)plm_get_time(stream->plm)-time<tick
			);
			
			if (frame!=NULL) {
				plm_frame_to_rgb(
					frame,
					stream->buffer,
					(int)width*3
				);
				
				glBindTexture(
					GL_TEXTURE_2D,
					textures[stream->texture_id]
				);
				glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					0,
					0,
					(GLsizei)width,
					(GLsizei)height,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					stream->buffer
				);
				glBindTexture(GL_TEXTURE_2D,0);
			}
		}
	}
}

ice_uint ice_video_init(
	ice_uint width,
	ice_uint height
) {
	visual = DMesaCreateVisual(
		width,  // X res
		height, // Y res
		16,     // BPP
		0,      // refresh rate: 0=default
		1,      // double-buffered
		1,      // RGB mode
		8,      // requested bits/alpha
		16,     // requested bits/depth
		0,      // requested bits/stencil
		0       // requested bits/accum
	);
	
	context = DMesaCreateContext(
		visual,
		NULL
	);
	
	buffers[0] = DMesaCreateBuffer(
		visual,
		0,     // X pos
		0,     // Y pos
		width, // X res
		height // Y res
	);
	
	buffers[1] = DMesaCreateBuffer(
		visual,
		0,     // X pos
		0,     // Y pos
		width, // X res
		height // Y res
	);
	
	DMesaMakeCurrent(
		context,
		buffers[0]
	);
	
	ice_log((ice_char *)"Initialized Voodoo graphics");
	
	textures=calloc(
		MAX_TEXTURES,
		sizeof(GLuint)
	);
	
	streams=calloc(
		MAX_STREAMS,
		sizeof(ice_video_stream)
	);
	
	arrays=calloc(
		MAX_ARRAYS,
		sizeof(ice_video_array)
	);
	
	return 0;
}

void ice_video_deinit() {	
	ice_video_texture_flush();
	ice_video_array_flush();
	ice_video_stream_flush();
	
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