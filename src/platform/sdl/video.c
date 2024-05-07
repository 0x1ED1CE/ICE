#include "ice.h"

#include <stdio.h>
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

static SDL_Window*   sdl_window = NULL;
static SDL_GLContext gl_context;

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

static GLuint *textures;

static ice_video_array  *arrays  = NULL;
static ice_video_stream *streams = NULL;

void ice_video_texture_flush() {
}

void ice_video_stream_flush() {
}

void ice_video_array_flush() {
}

ice_uint ice_video_width_get() {
	return 0;
}

ice_uint ice_video_height_get() {
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
	return 0;
}

void ice_video_array_delete(
	ice_uint array_id
) {
}

ice_uint ice_video_array_size_get(
	ice_uint array_id
) {
	return 0;
}

void ice_video_array_set(
	ice_uint array_id,
	ice_uint index,
	ice_real value
) {
}

ice_real ice_video_array_get(
	ice_uint array_id,
	ice_uint index
) {
	return 0;
}

ice_uint ice_video_model_load(
	ice_uint file_id
) {
	return 0;
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
	return 0;
}

void ice_video_stream_delete(
	ice_uint stream_id
) {
}

ice_real ice_video_stream_length_get(
	ice_uint stream_id
) {
	return 0;
}

ice_real ice_video_stream_width_get(
	ice_uint stream_id
) {
	return 0;
}

ice_real ice_video_stream_height_get(
	ice_uint stream_id
) {
	return 0;
}

ice_real ice_video_stream_position_get(
	ice_uint stream_id
) {
	return 0;
}

void ice_video_stream_position_set(
	ice_uint stream_id,
	ice_real position
) {
}

ice_uint ice_video_stream_state_get(
	ice_uint stream_id
) {
	return 0;
}

void ice_video_stream_state_set(
	ice_uint stream_id,
	ice_uint state
) {
}

void ice_video_depth_set(
	ice_uint state
) {
}

ice_uint ice_video_depth_get() {
	return 0;
}

void ice_video_buffer(
	ice_real tick
) {
}

ice_uint ice_video_init(
	ice_uint width,
	ice_uint height
) {
	if (sdl_window!=NULL) {
		ice_log((ice_char *)"SDL window is already initialized!");

		return 1;
	}

	SDL_GL_LoadLibrary(NULL);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,1);
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

	return 0;
}

void ice_video_deinit() {
}