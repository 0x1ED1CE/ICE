#include "lib/glad/glad.h"

static SDL_Window* sdl_window = NULL;
static SDL_GLContext gl_context;

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
	ice_sint s_by,
	ice_char c_ar,
	ice_char c_ag,
	ice_char c_ab,
	ice_char c_aa
) {
}

void ice_video_texture_triangle_draw(
	ice_uint texture_id,
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
	ice_char c_ar,
	ice_char c_ag,
	ice_char c_ab,
	ice_char c_aa,
	ice_char c_br,
	ice_char c_bg,
	ice_char c_bb,
	ice_char c_ba,
	ice_char c_cr,
	ice_char c_cg,
	ice_char c_cb,
	ice_char c_ca
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

void iice_video_model_draw(
	ice_uint texture_id,
	ice_uint projection_array_id,
	ice_uint model_array_id
) {
}

void ice_video_texture_flush() {
}

void ice_video_array_flush() {
}

void ice_video_buffer() {
}

ice_uint ice_video_init(
	ice_uint width,
	ice_uint height
) {
	if (sdl_window!=NULL) {
		ice_log((ice_char *)"SDL window is already initialized!");

		return -1;
	}

	SDL_GL_LoadLibrary(NULL);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,2);
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

		return -1;
	}

	gl_context = SDL_GL_CreateContext(sdl_window);

	if (gl_context==NULL) {
		ice_log((ice_char *)"Failed to create OpenGL context");

		return -1;
	}

	return 0;
}

void ice_video_deinit() {
}