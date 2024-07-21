#include "ice.h"

#include <SDL2/SDL.h>

#include "lib/glad/glad.h"

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

const char vertex_shader[]=
"#version 210 core \
layout (location = 0) in vec3 vertex_coord; \
layout (location = 1) in vec2 texture_coord; \
out vec2 TexCoord; \
uniform mat4 projection; \
void main() { \
	gl_Position = projection*vec4(vertex_coord,1.0f); \
	TexCoord    = texture_coord; \
}";

const char pixel_shader[]=
"#version 210 core \
out vec4 FragColor; \
in  vec2 TexCoord; \
uniform sampler2D texture_0; \
void main() { \
	FragColor = texture(texture_0,TexCoord); \
}";

static SDL_Window*   sdl_window = NULL;
static SDL_GLContext gl_context;

typedef struct {
	ice_uint type;
	GLuint   vbo;
	GLfloat *local;
} ice_video_array;

typedef struct {
	plm_t   *plm;
	uint8_t *buffer;
	ice_uint texture_id;
} ice_video_stream;

static GLuint           *textures = NULL;
static ice_video_array  *arrays   = NULL;
static ice_video_stream *streams  = NULL;

ice_uint ice_video_init(
	ice_uint width,
	ice_uint height
) {
	{ //Initialize SDL & OpenGL
		SDL_GL_LoadLibrary(NULL);

		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);

		SDL_GL_SetSwapInterval(1); //V-Sync

		sdl_window = SDL_CreateWindow(
			"ICE",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width,
			height,
			SDL_WINDOW_OPENGL
		);

		if (sdl_window==NULL) {
			char error_msg[256];

			sprintf(
				error_msg,
				"Failed to initialize SDL window: %s",
				SDL_GetError()
			);

			ice_log((ice_char *)error_msg);

			return 1;
		}

		gl_context = SDL_GL_CreateContext(sdl_window);

		if (gl_context==NULL) {
			ice_log((ice_char *)"Failed to create OpenGL context");

			return 1;
		}
		
		gladLoadGLLoader(SDL_GL_GetProcAddress);
		
		{
			char opengl_msg[256];

			sprintf(
				opengl_msg,
				"OpenGL version: %s",
				glGetString(GL_VERSION)
			);
			ice_log((ice_char *)opengl_msg);
			
			sprintf(
				opengl_msg,
				"OpenGL backend: %s",
				glGetString(GL_RENDERER)
			);
			ice_log((ice_char *)opengl_msg);
		}
	}
	
	{ //Allocate slots
		textures = calloc(
			MAX_TEXTURES,
			sizeof(GLuint)
		);
		
		arrays = calloc(
			MAX_ARRAYS,
			sizeof(ice_video_array)
		);
		
		streams = calloc(
			MAX_STREAMS,
			sizeof(ice_video_stream)
		);
		
		if (
			textures == NULL ||
			arrays   == NULL ||
			streams  == NULL
		) {
			ice_log((ice_char *)"Failed to allocate video slots!");
			
			return 1;
		}
	}
	
	{ //Setup default OpenGL states
		glEnable(GL_TEXTURE_2D);
		glCullFace(GL_BACK);
	}

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
	
	if (streams!=NULL) {
		free(streams);
		streams=NULL;
	}
}

void ice_video_buffer(
	ice_real tick
) {
	//SDL_GL_SwapBuffers();
}

void ice_video_clear(
	ice_uint attribute
) {
	switch(attribute) {
		case ICE_VIDEO_BUFFER_COLOR:
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);
			
			break;
		case ICE_VIDEO_BUFFER_DEPTH:
			glClear(GL_DEPTH_BUFFER_BIT);
	}
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

ice_uint ice_video_width_get() {
	int width;
	int height;
	
	SDL_GetWindowSize(
		sdl_window,
		&width,
		&height
	);
	
	return (ice_uint)width;
}

ice_uint ice_video_height_get() {
	int width;
	int height;
	
	SDL_GetWindowSize(
		sdl_window,
		&width,
		&height
	);
	
	return (ice_uint)height;
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
				GL_NEAREST
			);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MAG_FILTER,
				GL_NEAREST
			);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				format,
				(GLsizei)width,
				(GLsizei)height,
				0,
				format,
				GL_UNSIGNED_BYTE,
				NULL
			);
			
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
	
	glBindTexture(
		GL_TEXTURE_2D,
		0
	);
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
	
	glBindTexture(
		GL_TEXTURE_2D,
		textures[texture_id]
	);
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
	
	glBindTexture(
		GL_TEXTURE_2D,
		textures[texture_id]
	);
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
}

ice_uint ice_video_array_new(
	ice_uint type,
	ice_uint size
) {
	if (
		arrays == NULL ||
		type   == 0
	) {
		ice_log((ice_char *)"Cannot create invalid video array object");
		
		return 0;
	}
	
	ice_uint         array_id = 0;
	ice_video_array *array    = NULL;
	
	for (ice_uint i=0; i<MAX_ARRAYS; i++) {
		if (arrays[i].vbo==0) {
			array_id = i;
			array    = &arrays[i];
			
			break;
		}
	}
	
	if (array==NULL) {
		return 0;
	}
	
	glGenBuffers(1,&array->vbo);
	
	if (array->vbo==0) {
		ice_log((ice_char *)"Failed to allocate video array object");
		
		return 0;
	}
	
	array->type = type;
	
	glBindBuffer(
		GL_ARRAY_BUFFER,
		array->vbo
	);
	glBufferData(
		GL_ARRAY_BUFFER,
		size*sizeof(GLfloat),
		NULL,
		GL_DYNAMIC_DRAW
	);
	
	return array_id;
}

void ice_video_array_delete(
	ice_uint array_id
) {
	if (
		arrays==NULL ||
		array_id>MAX_ARRAYS ||
		arrays[array_id].vbo==0
	) {
		return;
	}
	
	ice_video_array *array = &arrays[array_id];
	
	glBindBuffer(
		GL_ARRAY_BUFFER,
		0
	);
	glDeleteBuffers(
		1,
		&array->vbo
	);
	
	array->type = 0;
	array->vbo  = 0;
}

ice_uint ice_video_array_size_get(
	ice_uint array_id
) {
	if (
		arrays==NULL ||
		array_id>MAX_ARRAYS ||
		arrays[array_id].vbo==0
	) {
		return 0;
	}
	
	ice_video_array *array = &arrays[array_id];
	GLint            array_size;
	
	glBindBuffer(
		GL_ARRAY_BUFFER,
		array->vbo
	);
	glGetBufferParameteriv(
		GL_ARRAY_BUFFER,
		GL_BUFFER_SIZE,
		&array_size
	);
	
	return array_size/sizeof(GLfloat);
}

void ice_video_array_set(
	ice_uint array_id,
	ice_uint index,
	ice_real value
) {
	if (
		arrays==NULL ||
		array_id>MAX_ARRAYS ||
		arrays[array_id].vbo==0
	) {
		return;
	}
	
	ice_video_array *array    = &arrays[array_id];
	GLfloat          gl_value = (GLfloat)value;
	
	glBindBuffer(
		GL_ARRAY_BUFFER,
		array->vbo
	);
	glBufferSubData(
		GL_ARRAY_BUFFER,
		(GLint)index*sizeof(GLfloat),
		sizeof(GLfloat),
		&gl_value
	);
}

ice_real ice_video_array_get(
	ice_uint array_id,
	ice_uint index
) {
	if (
		arrays==NULL ||
		array_id>MAX_ARRAYS ||
		arrays[array_id].vbo==0
	) {
		return 0;
	}
	
	ice_video_array *array = &arrays[array_id];
	GLfloat          gl_value;
	
	glBindBuffer(
		GL_ARRAY_BUFFER,
		array->vbo
	);
	glGetBufferSubData(
		GL_ARRAY_BUFFER,
		(GLint)index*sizeof(GLfloat),
		sizeof(GLfloat),
		&gl_value
	);
	
	return (ice_real)gl_value;
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
		ICE_VIDEO_ARRAY_MODEL_3,
		(ice_uint)mat_count_attributes(
			filename,
			MAT_ATTRIBUTE_FACE
		)*3
	);
	
	for (ice_uint m=0; faces.data!=NULL; m+=3) {
		ice_uint vertex_id = ice_video_array_new(
			ICE_VIDEO_ARRAY_POSITION_3,
			(ice_uint)faces.size
		);
		
		ice_uint texture_id = ice_video_array_new(
			ICE_VIDEO_ARRAY_TEXTURE_2,
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

void ice_video_model_draw(
	ice_uint model_array_id,
	ice_uint projection_array_id,
	ice_real color_r,
	ice_real color_g,
	ice_real color_b,
	ice_real color_a
) {
	
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
		return ICE_VIDEO_STREAM_PAUSED;
	}
	
	ice_video_stream *stream = &streams[stream_id];
	
	if (plm_get_loop(stream->plm)) {
		return ICE_VIDEO_STREAM_LOOPING;
	} else if(plm_get_video_enabled(stream->plm)) {
		return ICE_VIDEO_STREAM_PLAYING;
	}
	
	return ICE_VIDEO_STREAM_PAUSED;
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
		case ICE_VIDEO_STREAM_PLAYING:
			plm_set_video_enabled(
				stream->plm,
				TRUE
			);
			plm_set_loop(
				stream->plm,
				FALSE
			);
			
			break;
		case ICE_VIDEO_STREAM_LOOPING:
			plm_set_video_enabled(
				stream->plm,
				TRUE
			);
			plm_set_loop(
				stream->plm, 
				TRUE
			);
			
			break;
		default:
			plm_set_video_enabled(
				stream->plm,
				FALSE
			);
			plm_set_loop(
				stream->plm,
				FALSE
			);
	}
}