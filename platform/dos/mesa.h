#include "ice.h"

#include <stdio.h>
#include <stdint.h>

#include "lib/mesa/include/GL/gl.h"
#include "lib/mesa/include/GL/glu.h"
#include "lib/mesa/include/GL/dmesa.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/stb_image.h"

#define PL_MPEG_IMPLEMENTATION
#include "lib/plmpeg/plmpeg.h"

#define MAT_IMPLEMENTATION
#include "lib/mat/mat.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define MAX_TEXTURES 256
#define MAX_ARRAYS   256
#define MAX_MODELS   128

#define STREAM_BUFFER_SIZE (256*256*3)

typedef struct {
	ice_uint asset_id;
	GLuint   gl_texture_id;
	plm_t   *plm;
} ice_video_texture;

typedef struct {
	ice_uint size;
	GLfloat *data;
} ice_video_array;

typedef struct {
	ice_uint  asset_id;
	ice_uint  mesh_id;
	GLuint    gl_display_id;
	mat_mesh *mesh;
} ice_video_model;

static unsigned int screen_width;
static unsigned int screen_height;
static unsigned int current_buffer;

static DMesaVisual  visual;
static DMesaContext context;
static DMesaBuffer  buffers[2];

static ice_video_texture *textures = NULL;
static ice_video_array   *arrays   = NULL;
static ice_video_model   *models   = NULL;

static uint8_t *stream_buffer;

ice_uint ice_video_init() {
	screen_width  = 640;
	screen_height = 480;

	// Initialize OpenGL
	ice_log((ice_char *)"Initializing OpenGL");

	visual = DMesaCreateVisual(
		screen_width,  // X res
		screen_height, // Y res
		16,            // BPP
		0,             // refresh rate: 0=default
		1,             // double-buffered
		1,             // RGB mode
		8,             // requested bits/alpha
		16,            // requested bits/depth
		0,             // requested bits/stencil
		0              // requested bits/accum
	);

	context = DMesaCreateContext(
		visual,
		NULL
	);

	buffers[0] = DMesaCreateBuffer(
		visual,
		0,            // X pos
		0,            // Y pos
		screen_width, // X res
		screen_height // Y res
	);

	buffers[1] = DMesaCreateBuffer(
		visual,
		0,
		0,
		screen_width,
		screen_height
	);

	DMesaMakeCurrent(
		context,
		buffers[0]
	);

	current_buffer = 0;

	// Allocate slots
	ice_log((ice_char *)"Allocating graphics slots");

	textures = calloc(
		MAX_TEXTURES,
		sizeof(ice_video_texture)
	);

	arrays = calloc(
		MAX_ARRAYS,
		sizeof(ice_video_array)
	);
	
	models = calloc(
		MAX_MODELS,
		sizeof(ice_video_model)
	);

	if (
		textures == NULL ||
		arrays   == NULL ||
		models   == NULL
	) {
		ice_log((ice_char *)"Failed to allocate graphics slots!");

		return 1;
	}

	// Allocate stream buffer
	stream_buffer = malloc(STREAM_BUFFER_SIZE);

	if (stream_buffer==NULL) {
		ice_log((ice_char *)"Failed to allocate stream buffer!");

		return 1;
	}

	// Setup default OpenGL states
	glEnable(GL_TEXTURE_2D);
	glCullFace(GL_BACK);
	glClearColor(0,0,0,0);
	glClear(
		GL_COLOR_BUFFER_BIT|
		GL_DEPTH_BUFFER_BIT
	);
	glTexGeni(
		GL_S,
		GL_TEXTURE_GEN_MODE,
		GL_SPHERE_MAP
	);
	glTexGeni(
		GL_T,
		GL_TEXTURE_GEN_MODE,
		GL_SPHERE_MAP
	);

	// Generate default checkerboard texture
	unsigned char *temp_data = malloc(8*8*3);

	for (unsigned int y=0; y<8; y++) {
		for (unsigned int x=0; x<8; x++) {
			unsigned int i = (y*8+x)*3;
			unsigned int c = i%6==0;

			if (y%2==0) {
				temp_data[i]   = c?0:251;
				temp_data[i+1] = c?0:62;
				temp_data[i+2] = c?0:249;
			} else {
				temp_data[i]   = c?251:0;
				temp_data[i+1] = c?62:0;
				temp_data[i+2] = c?249:0;
			}
		}
	}

	glGenTextures(
		1,
		&textures[0].gl_texture_id
	);
	glBindTexture(
		GL_TEXTURE_2D,
		textures[0].gl_texture_id
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
		GL_RGB,
		8,
		8,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		temp_data
	);
	glBindTexture(GL_TEXTURE_2D,0);

	GLuint gl_error=glGetError();

	if (gl_error) {
		ice_char msg[128];

		sprintf(
			(char *)msg,
			"Failed to create default texture: %u",
			(unsigned int)gl_error
		);

		ice_log(msg);
	}

	free(temp_data);

	return 0;
}

void ice_video_deinit() {
	ice_log((ice_char *)"Flushing graphics slots");

	ice_video_texture_flush();
	ice_video_array_flush();
	ice_video_model_flush();

	if (textures!=NULL) {
		free(textures);
		textures=NULL;
	}

	if (arrays!=NULL) {
		free(arrays);
		arrays=NULL;
	}

	if (models!=NULL) {
		free(models);
		models=NULL;
	}

	if (stream_buffer!=NULL) {
		free(stream_buffer);
		stream_buffer=NULL;
	}

	ice_log((ice_char *)"Destroying OpenGL contexts");

	DMesaDestroyBuffer(buffers[0]);
	DMesaDestroyBuffer(buffers[1]);
	DMesaDestroyContext(context);
	DMesaDestroyVisual(visual);
}

void ice_video_update() {
	if (current_buffer) {
		current_buffer=0;
	} else {
		current_buffer=1;
	}

	DMesaSwapBuffers(buffers[current_buffer]);
}

ice_uint ice_video_width_get() {
	return screen_width;
}

ice_uint ice_video_height_get() {
	return screen_height;
}

void ice_video_clear(
	ice_uint attribute
) {
	switch(attribute) {
		case ICE_VIDEO_CLEAR_COLOR:
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);

			break;
		case ICE_VIDEO_CLEAR_DEPTH:
			glClear(GL_DEPTH_BUFFER_BIT);
	}
}

ice_uint ice_video_texture_load(
	ice_uint asset_id
) {
	if (asset_id==0) {
		return 0;
	}

	// Check if texture is already loaded
	for (ice_uint id=1; id<MAX_TEXTURES; id++) {
		if (
			textures[id].asset_id==asset_id &&
			textures[id].plm==NULL
		) {
			return id;
		}
	}

	ice_uint           texture_id = 0;
	ice_video_texture *texture    = NULL;

	// Otherwise find an available slot
	for (ice_uint id=1; id<MAX_TEXTURES; id++) {
		if (textures[id].asset_id==0) {
			texture_id = id;
			texture    = &textures[id];

			break;
		}
	}

	if (texture==NULL) {
		ice_log((ice_char *)"No texture slot available");

		return 0;
	}

	int            width;
	int            height;
	int            bpp;
	GLuint         gl_texture_id = 0;
	plm_t         *plm           = NULL;
	unsigned char *data          = NULL;

	// Attempt to load image
	char filename[32];

	sprintf(
		filename,
		"%u.png",
		(unsigned int)asset_id
	);

	data = stbi_load(
		filename,
		&width,
		&height,
		&bpp,
		STBI_rgb_alpha
	);

	// Check if it is a video file
	if (data==NULL) {
		sprintf(
			filename,
			"%u.mpg",
			(unsigned int)asset_id
		);

		plm=plm_create_with_filename(filename);

		if (plm==NULL) {
			ice_char msg[64];
			sprintf(
				(char *)msg,
				"Failed to load texture: %u",
				(unsigned int)asset_id
			);
			ice_log(msg);

			return 0;
		}

		plm_set_audio_enabled(
			plm,
			FALSE
		);
		plm_set_video_enabled(
			plm,
			TRUE
		);
		plm_set_loop(
			plm,
			FALSE
		);

		width  = plm_get_width(plm);
		height = plm_get_height(plm);
	}

	// Create OpenGL texture
	glGenTextures(1,&gl_texture_id);
	glBindTexture(
		GL_TEXTURE_2D,
		gl_texture_id
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
		(data==NULL)?GL_RGB:GL_RGBA,
		(GLsizei)width,
		(GLsizei)height,
		0,
		(data==NULL)?GL_RGB:GL_RGBA,
		GL_UNSIGNED_BYTE,
		data
	);

	GLuint gl_error=glGetError();

	if (data!=NULL) {
		stbi_image_free(data);
	}

	if (gl_error) {
		ice_char msg[128];

		sprintf(
			(char *)msg,
			"Failed to create OpenGL texture: %u",
			(unsigned int)gl_error
		);

		ice_log(msg);

		if (plm!=NULL) {
			plm_destroy(plm);
		}

		return 0;
	}

	texture->asset_id      = asset_id;
	texture->gl_texture_id = gl_texture_id;
	texture->plm           = plm;

	return texture_id;
}

void ice_video_texture_delete(
	ice_uint texture_id
) {
	if (
		textures==NULL ||
		texture_id>MAX_TEXTURES
	) {
		return;
	}

	ice_video_texture *texture = &textures[texture_id];

	if (texture->gl_texture_id!=0) {
		glBindTexture(GL_TEXTURE_2D,0); // Make sure we aren't binded to it
		glDeleteTextures(
			1,
			&texture->gl_texture_id
		);
	}

	if (texture->plm!=NULL) {
		plm_destroy(texture->plm);
	}

	texture->asset_id      = 0;
	texture->gl_texture_id = 0;
	texture->plm           = NULL;
}

void ice_video_texture_flush() {
	if (textures==NULL) {
		return;
	}

	for (ice_uint id=0; id<MAX_TEXTURES; id++) {
		ice_video_texture_delete(id);
	}
}

ice_uint ice_video_texture_asset_id_get(
	ice_uint texture_id
) {
	if (texture_id>MAX_TEXTURES) {
		return 0;
	}

	return textures[texture_id].asset_id;
}

ice_uint ice_video_texture_width_get(
	ice_uint texture_id
) {
	if (
		texture_id>MAX_TEXTURES ||
		textures[texture_id].gl_texture_id==0
	) {
		return 0;
	}

	GLint width;

	glBindTexture(
		GL_TEXTURE_2D,
		textures[texture_id].gl_texture_id
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
	if (
		texture_id>MAX_TEXTURES ||
		textures[texture_id].gl_texture_id==0
	) {
		return 0;
	}

	GLint height;

	glBindTexture(
		GL_TEXTURE_2D,
		textures[texture_id].gl_texture_id
	);
	glGetTexLevelParameteriv(
		GL_TEXTURE_2D,
		0,
		GL_TEXTURE_WIDTH,
		&height
	);

	return (ice_uint)height;
}

ice_real ice_video_texture_length_get(
	ice_uint texture_id
) {
	if (
		texture_id>MAX_TEXTURES ||
		textures[texture_id].plm==NULL
	) {
		return 0;
	}

	return (ice_real)plm_get_duration(textures[texture_id].plm);
}

ice_real ice_video_texture_position_get(
	ice_uint texture_id
) {
	if (
		texture_id>MAX_TEXTURES ||
		textures[texture_id].plm==NULL
	) {
		return 0;
	}

	return (ice_real)plm_get_time(textures[texture_id].plm);
}

void ice_video_texture_position_set(
	ice_uint texture_id,
	ice_real position
) {
	if (
		texture_id>MAX_TEXTURES ||
		textures[texture_id].plm==NULL
	) {
		return;
	}

	ice_video_texture *texture      = &textures[texture_id];
	plm_t            *plm           = texture->plm;
	GLuint            gl_texture_id = texture->gl_texture_id;
	ice_real          plm_time      = plm_get_time(plm);
	plm_frame_t      *frame         = NULL;

	position = MAX(position,0);
	position = MIN(
		position,
		plm_get_duration(plm)
	);

	if (
		position+0.2<plm_time ||
		position-0.2>plm_time
	) {
		frame=plm_seek_frame(
			plm,
			position,
			1
		);
	} else {
		while (plm_get_time(plm)<position) {
			frame=plm_decode_video(plm);

			if (frame==NULL) {
				break;
			}
		}
	}

	if (frame!=NULL) {
		int width  = plm_get_width(plm);
		int height = plm_get_height(plm);

		plm_frame_to_rgb(
			frame,
			stream_buffer,
			(int)width*3
		);

		glBindTexture(
			GL_TEXTURE_2D,
			gl_texture_id
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
			stream_buffer
		);
	}
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
	texture_id=(
		texture_id>MAX_TEXTURES ||
		textures[texture_id].gl_texture_id==0
	)?0:texture_id;

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA
	);
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
		textures[texture_id].gl_texture_id
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
	texture_id=(
		texture_id>MAX_TEXTURES ||
		textures[texture_id].gl_texture_id==0
	)?0:texture_id;

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
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
		textures[texture_id].gl_texture_id
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
		(GLfloat)d_ax*2.0-1.0, // Convert to cartesian
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
}

ice_uint ice_video_array_new(
	ice_uint size
) {
	if (size==0) {
		ice_log((ice_char *)"Cannot create array size of 0");

		return 0;
	}

	ice_uint         array_id = 0;
	ice_video_array *array    = NULL;

	for (ice_uint id=0; id<MAX_ARRAYS; id++) {
		if (arrays[id].data==NULL) {
			array_id = id;
			array    = &arrays[id];
		}
	}

	if (array==NULL) {
		ice_log((ice_char *)"No array slot available");

		return 0;
	}

	array->data = calloc(
		size,
		sizeof(GLfloat)
	);

	if (array->data==NULL) {
		ice_log((ice_char *)"Failed to allocate video array object");

		return 0;
	}

	array->size = size;

	return array_id;
}

void ice_video_array_delete(
	ice_uint array_id
) {
	if (
		array_id>MAX_ARRAYS ||
		arrays[array_id].data==NULL
	) {
		return;
	}

	ice_video_array *array = &arrays[array_id];

	free(array->data);

	array->data = NULL;
	array->size = 0;
}

void ice_video_array_flush() {
	if (arrays==NULL) {
		return;
	}

	for (ice_uint id=0; id<MAX_ARRAYS; id++) {
		ice_video_array_delete(id);
	}
}

ice_uint ice_video_array_size_get(
	ice_uint array_id
) {
	if (
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
	ice_uint asset_id,
	ice_uint mesh_id
) {
	if (asset_id==0) {
		return 0;
	}

	// Check if model is already loaded
	for (ice_uint id=1; id<MAX_MODELS; id++) {
		if (
			models[id].asset_id==asset_id &&
			models[id].mesh_id==mesh_id
		) {
			return id;
		}
	}

	ice_uint         model_id = 0;
	ice_video_model *model    = NULL;

	// Find an available slot
	for (ice_uint id=1; id<MAX_MODELS; id++) {
		if (models[id].asset_id==0) {
			model_id = id;
			model    = &models[id];

			break;
		}
	}

	if (model==NULL) {
		ice_log((ice_char *)"No model slot available");

		return 0;
	}

	// Attempt to load model
	char filename[32];

	sprintf(
		filename,
		"%u.mat",
		(unsigned int)asset_id
	);
	
	mat_mesh *mesh=mat_mesh_load(filename,mesh_id);
	// mat_mesh_normalize(mesh);

	if (
		mesh==NULL ||
		mesh->vert_data==NULL ||
		mesh->norm_data==NULL ||
		mesh->text_data==NULL ||
		mesh->face_data==NULL
	) {
		ice_char msg[128];

		sprintf(
			(char *)msg,
			"Failed to load model: %u",
			(unsigned int)asset_id
		);

		ice_log(msg);

		mat_mesh_free(mesh);

		return 0;
	}

	model->asset_id = asset_id;
	model->mesh_id  = mesh_id;

	if (mesh->skin_data==NULL) {
		// If model is static then compile into display list
		model->gl_display_id=glGenLists(1);

		glNewList(
			model->gl_display_id,
			GL_COMPILE
		);
		glBegin(GL_TRIANGLES);

		for (ice_uint f=0; f<mesh->face_size; f+=3) {
			ice_uint v = MIN(mesh->face_data[f]*3,mesh->vert_size-3);
			ice_uint n = MIN(mesh->face_data[f+1]*3,mesh->norm_size-3);
			ice_uint t = MIN(mesh->face_data[f+2]*2,mesh->text_size-2);

			glTexCoord2f(
				(GLfloat)mesh->text_data[t],
				(GLfloat)mesh->text_data[t+1]
			);
			glNormal3f(
				(GLfloat)mesh->norm_data[n],
				(GLfloat)mesh->norm_data[n+1],
				(GLfloat)mesh->norm_data[n+2]
			);
			glVertex3f(
				(GLfloat)mesh->vert_data[v],
				(GLfloat)mesh->vert_data[v+1],
				(GLfloat)mesh->vert_data[v+2]
			);
		}

		glEnd();
		glEndList();

		mat_mesh_free(mesh);
	} else {
		model->mesh = mesh;
	}

	return model_id;
}

void ice_video_model_delete(
	ice_uint model_id
) {
	if (
		model_id>MAX_MODELS ||
		models[model_id].asset_id==0
	) {
		return;
	}

	ice_video_model *model = &models[model_id];

	if (model->gl_display_id!=0) {
		glDeleteLists(model->gl_display_id,1);
	}

	if (model->mesh!=NULL) {
		mat_mesh_free(model->mesh);
	}

	model->asset_id      = 0;
	model->mesh_id       = 0;
	model->gl_display_id = 0;
	model->mesh          = NULL;
}

void ice_video_model_flush() {
	if (models==NULL) {
		return;
	}

	for (ice_uint id=0; id<MAX_MODELS; id++) {
		ice_video_model_delete(id);
	}
}

void ice_video_model_draw(
	ice_uint model_id,
	ice_uint pose_id,
	ice_uint projection_id,
	ice_uint texture_id,
	ice_uint effect,
	ice_real c_r,
	ice_real c_g,
	ice_real c_b,
	ice_real c_a
) {
	if (
		model_id>MAX_MODELS ||
		models[model_id].asset_id==0
	) {
		return;
	}

	if (
		projection_id>MAX_ARRAYS ||
		arrays[projection_id].data==NULL
	) {
		return;
	}

	texture_id=(
		texture_id>MAX_TEXTURES ||
		textures[texture_id].gl_texture_id==0
	)?0:texture_id;

	ice_video_model   *model      = &models[model_id];
	ice_video_array   *projection = &arrays[projection_id];
	ice_video_texture *texture    = &textures[texture_id];

	switch(effect) {
		case ICE_VIDEO_EFFECT_AMBIENT:
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glEnable(GL_COLOR_MATERIAL);

			break;
		case ICE_VIDEO_EFFECT_REFLECT:
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);

			break;
		case ICE_VIDEO_EFFECT_LATTICE:
			glDisable(GL_CULL_FACE);
			glPolygonMode(
				GL_FRONT_AND_BACK,
				GL_LINE
			);

			break;
		default:
			glEnable(GL_CULL_FACE);
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_EQUAL,1.0);
	glLoadMatrixf(projection->data);
	glBindTexture(
		GL_TEXTURE_2D,
		texture->gl_texture_id
	);

	glColor4f(
		(GLfloat)c_r,
		(GLfloat)c_g,
		(GLfloat)c_b,
		(GLfloat)c_a
	);

	if (model->gl_display_id!=0) {
		glCallList(model->gl_display_id);
	} else if (model->mesh!=NULL) {
		mat_mesh *mesh      = model->mesh;
		GLfloat  *pose_data = arrays[pose_id].data;

		glBegin(GL_TRIANGLES);

		for (ice_uint f=0; f<mesh->face_size; f+=3) {
			ice_uint v = mesh->face_data[f]*3;
			ice_uint n = mesh->face_data[f+1]*3;
			ice_uint t = mesh->face_data[f+2]*2;
			ice_uint s = mesh->skin_data[mesh->face_data[f]]*16;

			GLfloat vx = (GLfloat)mesh->vert_data[v];
			GLfloat vy = (GLfloat)mesh->vert_data[v+1];
			GLfloat vz = (GLfloat)mesh->vert_data[v+2];

			glTexCoord2f(
				(GLfloat)mesh->text_data[t],
				(GLfloat)mesh->text_data[t+1]
			);
			glNormal3f(
				(GLfloat)mesh->norm_data[n],
				(GLfloat)mesh->norm_data[n+1],
				(GLfloat)mesh->norm_data[n+2]
			);
			glVertex3f(
				vx*pose_data[s]+vy*pose_data[s+1]+vz*pose_data[s+2]+pose_data[s+3],
				vx*pose_data[s+4]+vy*pose_data[s+5]+vz*pose_data[s+6]+pose_data[s+7],
				vx*pose_data[s+8]+vy*pose_data[s+9]+vz*pose_data[s+10]+pose_data[s+11]
			);
		}

		glEnd();
	}

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glPolygonMode(
		GL_FRONT_AND_BACK,
		GL_FILL
	);
}
