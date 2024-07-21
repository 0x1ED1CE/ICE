/*
                GRAPHICS                                TEXT

15-bit   7-bit    Resolution   Colors   15-bit   7-bit    Columns   Rows
mode     mode                           mode     mode
number   number                         number   number
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
100h     -        640x400      256      108h     -        80        60
101h     -        640x480      256
                                        109h     -        132       25
102h     6Ah      800x600      16       10Ah     -        132       43
103h     -        800x600      256      10Bh     -        132       50
                                        10Ch     -        132       60
104h     -        1024x768     16
105h     -        1024x768     256

106h     -        1280x1024    16
107h     -        1280x1024    256

VESA Super VGA Standard VS911022-8

10Dh     -        320x200      32K   (1:5:5:5)
10Eh     -        320x200      64K   (5:6:5)
10Fh     -        320x200      16.8M (8:8:8)
110h     -        640x480      32K   (1:5:5:5)
111h     -        640x480      64K   (5:6:5)
112h     -        640x480      16.8M (8:8:8)
113h     -        800x600      32K   (1:5:5:5)
114h     -        800x600      64K   (5:6:5)
115h     -        800x600      16.8M (8:8:8)
116h     -        1024x768     32K   (1:5:5:5)
117h     -        1024x768     64K   (5:6:5)
118h     -        1024x768     16.8M (8:8:8)
119h     -        1280x1024    32K   (1:5:5:5)
11Ah     -        1280x1024    64K   (5:6:5)
11Bh     -        1280x1024    16.8M (8:8:8)
*/

#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include <dpmi.h>
#include <go32.h>
#include <sys/farptr.h>
#include <sys/movedata.h>

#include "lib/tinygl/include/GL/gl.h"
#include "lib/tinygl/include/zbuffer.h"
#include "lib/tinygl/include/zfeatures.h"

#include "ice.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/stb_image.h"

#ifndef PL_MPEG_IMPLEMENTATION
#define PL_MPEG_IMPLEMENTATION
#include "lib/plmpeg/plmpeg.h"
#endif

#include "lib/mat/mat.h"

#define VIDEO_ADDRESS 0xA0000
#define VIDEO_MODE    0x10E
#define VIDEO_BYTES   2

#define MAX_TEXTURES 256
#define MAX_STREAMS  8
#define MAX_ARRAYS   256

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

typedef struct { 
	unsigned char  VESASignature[4]    __attribute__ ((packed));
	unsigned short VESAVersion         __attribute__ ((packed));
	unsigned long  OEMStringPtr        __attribute__ ((packed));
	unsigned char  Capabilities[4]     __attribute__ ((packed));
	unsigned long  VideoModePtr        __attribute__ ((packed));
	unsigned short TotalMemory         __attribute__ ((packed));
	unsigned short OemSoftwareRev      __attribute__ ((packed));
	unsigned long  OemVendorNamePtr    __attribute__ ((packed));
	unsigned long  OemProductNamePtr   __attribute__ ((packed));
	unsigned long  OemProductRevPtr    __attribute__ ((packed));
	unsigned char  Reserved[222]       __attribute__ ((packed));
	unsigned char  OemData[256]        __attribute__ ((packed));
} VESA_INFO;

typedef struct {
	unsigned short ModeAttributes      __attribute__ ((packed));
	unsigned char  WinAAttributes      __attribute__ ((packed));
	unsigned char  WinBAttributes      __attribute__ ((packed));
	unsigned short WinGranularity      __attribute__ ((packed));
	unsigned short WinSize             __attribute__ ((packed));
	unsigned short WinASegment         __attribute__ ((packed));
	unsigned short WinBSegment         __attribute__ ((packed));
	unsigned long  WinFuncPtr          __attribute__ ((packed));
	unsigned short BytesPerScanLine    __attribute__ ((packed));
	unsigned short XResolution         __attribute__ ((packed));
	unsigned short YResolution         __attribute__ ((packed));
	unsigned char  XCharSize           __attribute__ ((packed));
	unsigned char  YCharSize           __attribute__ ((packed));
	unsigned char  NumberOfPlanes      __attribute__ ((packed));
	unsigned char  BitsPerPixel        __attribute__ ((packed));
	unsigned char  NumberOfBanks       __attribute__ ((packed));
	unsigned char  MemoryModel         __attribute__ ((packed));
	unsigned char  BankSize            __attribute__ ((packed));
	unsigned char  NumberOfImagePages  __attribute__ ((packed));
	unsigned char  Reserved_page       __attribute__ ((packed));
	unsigned char  RedMaskSize         __attribute__ ((packed));
	unsigned char  RedMaskPos          __attribute__ ((packed));
	unsigned char  GreenMaskSize       __attribute__ ((packed));
	unsigned char  GreenMaskPos        __attribute__ ((packed));
	unsigned char  BlueMaskSize        __attribute__ ((packed));
	unsigned char  BlueMaskPos         __attribute__ ((packed));
	unsigned char  ReservedMaskSize    __attribute__ ((packed));
	unsigned char  ReservedMaskPos     __attribute__ ((packed));
	unsigned char  DirectColorModeInfo __attribute__ ((packed));
	unsigned long  PhysBasePtr         __attribute__ ((packed));
	unsigned long  OffScreenMemOffset  __attribute__ ((packed));
	unsigned short OffScreenMemSize    __attribute__ ((packed));
	unsigned char  Reserved[206]       __attribute__ ((packed));
} MODE_INFO;

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

VESA_INFO vesa_info;
MODE_INFO mode_info;

static unsigned int screen_width;
static unsigned int screen_height;

static ZBuffer          *framebuffer = NULL;
static unsigned char    *screen      = NULL;
static GLuint           *textures    = NULL;
static ice_video_array  *arrays      = NULL;
static ice_video_stream *streams     = NULL;

int vesa_info_get() {
	__dpmi_regs r;
	unsigned long dosbuf;
	unsigned int c;
	
	dosbuf=__tb&0xFFFFF;

	for (c=0; c<sizeof(VESA_INFO); c++) {
		_farpokeb(_dos_ds,dosbuf+c,0);
	}

	dosmemput("VBE2", 4, dosbuf);

	r.x.ax = 0x4F00;
	r.x.di = dosbuf&0xF;
	r.x.es = (dosbuf>>4)&0xFFFF;
	__dpmi_int(0x10,&r);

	if (r.h.ah) {
		return -1;
	}

	dosmemget(
		dosbuf,
		sizeof(VESA_INFO),
		&vesa_info
	);

	if (strncmp(
		(const char *)(vesa_info.VESASignature),
		"VESA",
		4
	)!=0) {
		return -1;
	}

	return 0;
}

int vesa_mode_get(
	unsigned int mode
) {
	__dpmi_regs r;
	unsigned long dosbuf;
	unsigned int c;

	dosbuf = __tb&0xFFFFF;

	for (c=0; c<sizeof(MODE_INFO); c++) {
		_farpokeb(_dos_ds,dosbuf+c,0);
	}

	r.x.ax = 0x4F01;
	r.x.di = dosbuf&0xF;
	r.x.es = (dosbuf>>4)&0xFFFF;
	r.x.cx = mode;
	__dpmi_int(0x10,&r);

	if (r.h.ah) {
		return -1;
	}

	dosmemget(
		dosbuf,
		sizeof(MODE_INFO),
		&mode_info
	);

	return 0;
}

int vesa_mode_set(
	unsigned int mode_number
) {
	__dpmi_regs r;
	
	if (!mode_number) { //Return to text mode
		union REGS regs = {};
		regs.h.al       = 0x3;
		int86(0x10,&regs,&regs);
		
		//clrscr();
		
		return 0;
	}

	r.x.ax = 0x4F02;
	r.x.bx = mode_number;
	__dpmi_int(0x10,&r);
	
	if (r.h.ah)
		return -1;

	return 0;
}

void set_vesa_bank(
	unsigned int bank_number
) {
	__dpmi_regs r;
	r.x.ax = 0x4F05;
	r.x.bx = 0;
	r.x.dx = bank_number;
	__dpmi_int(0x10, &r);
}

void vesa_buffer(
	void *buffer,
	unsigned int screen_size
) {
	unsigned int bank_size        = mode_info.WinSize*1024;
	unsigned int bank_granularity = mode_info.WinGranularity*1024;
	unsigned int bank_number      = 0;
	unsigned int bank_offset      = bank_size/bank_granularity;
	unsigned int todo             = screen_size;
	unsigned int copy_size        = bank_size;
	
	while (todo>0) {
		set_vesa_bank(bank_number);
		
		if (todo>bank_size) {
			copy_size=bank_size;
		} else {
			copy_size=todo;
		}
		
		dosmemput(
			buffer,
			copy_size,
			VIDEO_ADDRESS
		);
		
		todo        -= copy_size;
		buffer      += copy_size;
		bank_number += bank_offset;
	}
}

ice_uint ice_video_init(
	ice_uint width,
	ice_uint height
) {
	//Initialize vesa
	vesa_mode_get(VIDEO_MODE);
	vesa_info_get();
	
	if (vesa_mode_set(VIDEO_MODE)) {
		ice_char msg[64];
		sprintf(
			(char *)msg,
			"Video mode %X is not supported!",
			VIDEO_MODE
		);
		ice_log(msg);
		
		return 1;
	}
	
	screen = calloc(
		width*
		height*
		VIDEO_BYTES,
		sizeof(unsigned char)
	);
	
	if (screen==NULL) {
		ice_log((ice_char *)"Failed to initialize vesa buffer!");
		
		return 1;
	}
	
	screen_width  = width;
	screen_height = height;
	
	//Initialize OpenGL
	framebuffer = ZB_open(
		(GLuint)width,
		(GLuint)height,
		ZB_MODE_5R6G5B,
		NULL
	);
	
	if (framebuffer==NULL) {
		ice_log((ice_char *)"Failed to initialize framebuffer!");
		
		return 1;
	}
	
	glInit(framebuffer);
	
	//Allocate slots
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
	
	//Setup default OpenGL states
	glEnable(GL_TEXTURE_2D);
	glCullFace(GL_BACK);
	
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
	
	if (screen!=NULL) {
		free(screen);
		screen=NULL;
	}
	
	if (framebuffer!=NULL) {
		ZB_close(framebuffer);
		glClose();
	}
	
	vesa_mode_set(0x000);
}

void ice_video_buffer(ice_real tick) {
	vesa_buffer(
		framebuffer->pbuf,
		(unsigned int)
		screen_width*
		screen_height*
		VIDEO_BYTES
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
				/*
				glBindTexture(
					GL_TEXTURE_2D,
					textures[stream->texture_id]
				);
				glReadPixels(
					0,
					0,
					(GLsizei)width,
					(GLsizei)height,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					stream->buffer
				);
				glBindTexture(GL_TEXTURE_2D,0);
				*/
			}
		}
	}
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
	return screen_width;
}

ice_uint ice_video_height_get() {
	return screen_height;
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
			/*
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
				3,
				(GLsizei)width, //Size must be power of 2 for GL1.1
				(GLsizei)height,
				0,
				format,
				GL_UNSIGNED_BYTE,
				NULL
			);
			glBindTexture(GL_TEXTURE_2D,0);
			*/
			
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
		STBI_rgb
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
		GL_RGB
	);
	
	if (texture_id==0) {
		stbi_image_free(data);
		
		return 0;
	}
	
	glBindTexture(
		GL_TEXTURE_2D,
		textures[texture_id]
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
		3,
		(GLsizei)width,
		(GLsizei)height,
		0,
		GL_RGB,
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
	
	GLint width = 0;
	
	/*
	glGetTexLevelParameteriv(
		GL_TEXTURE_2D,
		0,
		GL_TEXTURE_WIDTH,
		&width
	);
	*/
	
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
	
	GLint height = 0;
	
	/*
	glGetTexLevelParameteriv(
		GL_TEXTURE_2D,
		0,
		GL_TEXTURE_WIDTH,
		&height
	);
	*/
	
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
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	/*
	glBlendFunc(
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA
	);
	*/
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
		textures[texture_id]
	);
	glBegin(GL_QUADS);
	glColor3f(
		(GLfloat)c_ar,
		(GLfloat)c_ag,
		(GLfloat)c_ab
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
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
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
		textures[texture_id]
	);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(
		(GLfloat)s_ax,
		(GLfloat)s_ay
	);
	glColor3f(
		(GLfloat)c_ar,
		(GLfloat)c_ag,
		(GLfloat)c_ab
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
	glColor3f(
		(GLfloat)c_br,
		(GLfloat)c_bg,
		(GLfloat)c_bb
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
	glColor3f(
		(GLfloat)c_cr,
		(GLfloat)c_cg,
		(GLfloat)c_cb
	);
	glVertex3f(
		(GLfloat)d_cx*2.0-1.0,
		1.0-(GLfloat)d_cy*2.0,
		0.0
	);
	glEnd();
	glFlush();
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
		projection_array->type!=ICE_VIDEO_ARRAY_MATRIX_16 ||
		model_array==NULL ||
		model_array->type!=ICE_VIDEO_ARRAY_MODEL_3
	) {
		return;
	}
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	//glAlphaFunc(GL_EQUAL,1.0);
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
				position_array->type!=ICE_VIDEO_ARRAY_POSITION_3 ||
				texture_array==NULL ||
				texture_array->type!=ICE_VIDEO_ARRAY_TEXTURE_2
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
	
	glBindTexture(
		GL_TEXTURE_2D,
		textures[texture_id]
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
		3,
		(GLsizei)width,
		(GLsizei)height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		NULL
	);
	glBindTexture(
		GL_TEXTURE_2D,
		0
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