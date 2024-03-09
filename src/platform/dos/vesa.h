//THIS IS DEPRECATED IN FAVOR OF MESA

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

#include <string.h>
#include <malloc.h>
#include <dos.h>
#include <dpmi.h>
#include <go32.h>
#include <sys/farptr.h>
#include <sys/movedata.h>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/stb_image.h"
#include "lib/etch/etch.h"

#define VIDEO_ADDRESS 0xA0000
//#define VIDEO_MODE    0x110
//#define VIDEO_WIDTH   640
//#define VIDEO_HEIGHT  480
#define VIDEO_MODE    0x10D
#define VIDEO_WIDTH   320
#define VIDEO_HEIGHT  200
#define VIDEO_BYTES   2

#define MAX_TEXTURES 256

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
	unsigned char  type;
	unsigned char *data;
	float         *z_data;
	unsigned int   width;
	unsigned int   height;
} video_texture;

VESA_INFO vesa_info;
MODE_INFO mode_info;

unsigned char *framebuffer = NULL;

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
	unsigned char *buffer,
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

ice_uint ice_video_texture_new(
	ice_uint width,
	ice_uint height,
	ice_uint attributes
) {
	return etch_texture_new(
		(etch_uint)width,
		(etch_uint)height,
		(etch_char)attributes&ICE_TEXTURE_ATTRIBUTE_DEPTH
	);
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
		ice_char msg[256];
		sprintf(
			(char *)msg,
			"Failed to load texture: %u",
			file_id
		);
		ice_log(msg);
		
		return 0;
	}
	
	etch_uint texture_id=etch_texture_new(
		(etch_uint)width,
		(etch_uint)height,
		0
	);
	
	if (texture_id==0) {
		stbi_image_free(data);
		return 0;
	}
	
	memcpy(
		textures[texture_id].data,
		data,
		width*height*ETCH_PIXEL_SIZE
	);
	
	stbi_image_free(data);
	
	return (ice_uint)texture_id;
}

void ice_video_texture_delete(
	ice_uint texture_id
) {
	etch_texture_free(
		(etch_uint)texture_id
	);
}

ice_uint ice_video_texture_width_get(
	ice_uint texture_id
) {
	return (ice_uint)etch_texture_width_get(
		(etch_uint)texture_id
	);
}

ice_uint ice_video_texture_height_get(
	ice_uint texture_id
) {
	return (ice_uint)etch_texture_height_get(
		(etch_uint)texture_id
	);
}

void ice_video_texture_clear(
	ice_uint texture_id
) {
	etch_texture_clear(
		(etch_uint)texture_id
	);
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
	ice_real d_width=(ice_real)ice_video_texture_width_get(
		d_texture_id
	);
	ice_real d_height=(ice_real)ice_video_texture_height_get(
		d_texture_id
	);
	ice_real s_width=(ice_real)ice_video_texture_width_get(
		s_texture_id
	);
	ice_real s_height=(ice_real)ice_video_texture_height_get(
		s_texture_id
	);
	
	etch_texture_rectangle_draw(
		(etch_uint)d_texture_id,
		(etch_uint)s_texture_id,
		(etch_sint)(d_ax*d_width),
		(etch_sint)(d_ay*d_height),
		(etch_sint)(d_bx*d_width),
		(etch_sint)(d_by*d_height),
		(etch_sint)(s_ax*s_width),
		(etch_sint)(s_ay*s_height),
		(etch_sint)(s_bx*s_width),
		(etch_sint)(s_by*s_height),
		(etch_char)(c_ar*255),
		(etch_char)(c_ag*255),
		(etch_char)(c_ab*255),
		(etch_char)(c_aa*255)
	);
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
	ice_real d_width=(ice_real)ice_video_texture_width_get(
		d_texture_id
	);
	ice_real d_height=(ice_real)ice_video_texture_height_get(
		d_texture_id
	);
	ice_real s_width=(ice_real)ice_video_texture_width_get(
		s_texture_id
	);
	ice_real s_height=(ice_real)ice_video_texture_height_get(
		s_texture_id
	);
	
	etch_texture_triangle_draw(
		(etch_uint)d_texture_id,
		(etch_uint)s_texture_id,
		(etch_sint)(d_ax*d_width),
		(etch_sint)(d_ay*d_height),
		(etch_sint)d_az,
		(etch_sint)(d_bx*d_width),
		(etch_sint)(d_by*d_height),
		(etch_sint)d_bz,
		(etch_sint)(d_cx*d_width),
		(etch_sint)(d_cy*d_height),
		(etch_sint)d_cz,
		(etch_sint)(s_ax*s_width),
		(etch_sint)(s_ay*s_height),
		(etch_sint)(s_bx*s_width),
		(etch_sint)(s_by*s_height),
		(etch_sint)(s_cx*s_width),
		(etch_sint)(s_cy*s_height),
		(etch_char)(c_ar*255),
		(etch_char)(c_ag*255),
		(etch_char)(c_ab*255),
		(etch_char)(c_aa*255),
		(etch_char)(c_br*255),
		(etch_char)(c_bg*255),
		(etch_char)(c_bb*255),
		(etch_char)(c_ba*255),
		(etch_char)(c_cr*255),
		(etch_char)(c_cg*255),
		(etch_char)(c_cb*255),
		(etch_char)(c_ca*255)
	);
}

ice_uint ice_video_vertex_new(
	ice_uint size
) {
	return (ice_uint)etch_vertex_new(
		(etch_uint)size
	);
}

void ice_video_vertex_delete(
	ice_uint vertex_id
) {
	etch_vertex_free(
		(etch_uint)vertex_id
	);
}

ice_uint ice_video_vertex_size_get(
	ice_uint vertex_id
) {
	return (ice_uint)etch_vertex_size_get(
		(etch_uint)vertex_id
	);
}

void ice_video_vertex_set_uint(
	ice_uint vertex_id,
	ice_uint index,
	ice_uint value
) {
	etch_vertex_set_uint(
		(etch_uint)vertex_id,
		(etch_uint)index,
		(etch_uint)value
	);
}

ice_uint ice_video_vertex_get_uint(
	ice_uint vertex_id,
	ice_uint index
) {
	return (ice_uint)etch_vertex_get_uint(
		(etch_uint)vertex_id,
		(etch_uint)index
	);
}

void ice_video_vertex_set_sint(
	ice_uint vertex_id,
	ice_uint index,
	ice_sint value
) {
	etch_vertex_set_sint(
		(etch_uint)vertex_id,
		(etch_uint)index,
		(etch_sint)value
	);
}

ice_sint ice_video_vertex_get_sint(
	ice_uint vertex_id,
	ice_uint index
) {
	return (ice_sint)etch_vertex_get_sint(
		(etch_uint)vertex_id,
		(etch_uint)index
	);
}

void ice_video_vertex_set_real(
	ice_uint vertex_id,
	ice_uint index,
	ice_real value
) {
	etch_vertex_set_real(
		(etch_uint)vertex_id,
		(etch_uint)index,
		(etch_real)value
	);
}

ice_real ice_video_vertex_get_real(
	ice_uint vertex_id,
	ice_uint index
) {
	return (ice_real)etch_vertex_get_real(
		(etch_uint)vertex_id,
		(etch_uint)index
	);
}

void ice_video_vertex_texture_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_real render_matrix[4][4],
	ice_uint position_vertex_id,
	ice_uint texture_vertex_id,
	ice_uint face_vertex_id
) {
	etch_vertex_texture_draw(
		(etch_uint)d_texture_id,
		(etch_uint)s_texture_id,
		(etch_real (*)[4])render_matrix,
		(etch_uint)position_vertex_id,
		(etch_uint)texture_vertex_id,
		(etch_uint)face_vertex_id
	);
}

void ice_video_texture_flush() {
	etch_texture_flush();
}

void ice_video_vertex_flush() {
	etch_vertex_flush();
}

void ice_video_buffer() {
	if (
		textures==NULL ||
		framebuffer==NULL
	) {
		return;
	}
	
	etch_texture *texture = &textures[0];
	etch_char    *data    = texture->data;
	
	unsigned short pixel;
	
	unsigned int d_index = 0;
	unsigned int s_index = 0;
	
	for (
		;
		d_index < VIDEO_WIDTH*VIDEO_HEIGHT*VIDEO_BYTES;
		d_index += VIDEO_BYTES
	) {
		pixel=(unsigned short)
			((data[s_index]*31/255)<<10)| 
			((data[s_index+1]*31/255)<<5)| 
			(data[s_index+2]*31/255);
		
		framebuffer[d_index]   = (unsigned char)(pixel&0xFF);
		framebuffer[d_index+1] = (unsigned char)(pixel>>8);
		
		s_index += ETCH_PIXEL_SIZE;
	}
	
	vesa_buffer(
		framebuffer,
		VIDEO_WIDTH*
		VIDEO_HEIGHT*
		VIDEO_BYTES
	);
}

ice_uint ice_video_init(
	ice_uint width,
	ice_uint height
) {
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
	
	framebuffer=(unsigned char *)calloc(
		VIDEO_WIDTH*VIDEO_HEIGHT,
		VIDEO_BYTES
	);
	
	if (framebuffer==NULL) {
		ice_log((ice_char*)"Failed to allocate framebuffer!");
		
		vesa_mode_set(0x000);
		
		return 2;
	}
	
	if (
		etch_texture_init() ||
		etch_vertex_init()
	) {
		ice_log((ice_char*)"Failed to initialize graphics library!");
		
		etch_texture_deinit();
		etch_vertex_deinit();
		
		free(framebuffer);
		
		vesa_mode_set(0x000);
		
		return 3;
	}
	
	ice_video_texture_new(
		VIDEO_WIDTH,
		VIDEO_HEIGHT,
		ICE_TEXTURE_ATTRIBUTE_WRITE |
		ICE_TEXTURE_ATTRIBUTE_COLOR |
		ICE_TEXTURE_ATTRIBUTE_DEPTH
	);
	
	return 0;
}

void ice_video_deinit() {	
	etch_texture_deinit();
	etch_vertex_deinit();
	
	if (framebuffer!=NULL) {
		free(framebuffer);
		framebuffer=NULL;
	}
}