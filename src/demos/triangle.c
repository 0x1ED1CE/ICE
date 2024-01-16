#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "ice.h"

int main() {
	if (ice_init()) {
		return -2;
	}
	
	ice_uint texture_1=ice_video_texture_load(0);
	
	ice_video_texture_triangle_draw(
		0, 2,
		159, 19,  1,
		39,  179, 1,
		279, 179, 1,
		0, 0,
		0, 0,
		0, 0,
		255, 0,   0,   255,
		0,   255, 0,   255,
		0,   0,   255, 255
	);
	ice_video_buffer();
	getch();
	
	ice_video_texture_clear(0);
	
	ice_sint ax;
	ice_sint ay;
	ice_sint az;
	ice_sint bx;
	ice_sint by;
	ice_sint bz;
	ice_sint cx;
	ice_sint cy;
	ice_sint cz;
	ice_char ar;
	ice_char ag;
	ice_char ab;
	ice_char aa=255;
	
	ice_real start=ice_clock_get();
	
	unsigned int draws=10000;
	
	for (unsigned int i=0; i<draws; i++) {
		ax=(ice_sint)rand()%320;
		ay=(ice_sint)rand()%200;
		az=(ice_sint)rand()%65535;
		bx=(ice_sint)rand()%320;
		by=(ice_sint)rand()%200;
		bz=(ice_sint)rand()%65535;
		cx=(ice_sint)rand()%320;
		cy=(ice_sint)rand()%200;
		cz=(ice_sint)rand()%65535;
		ar=(ice_sint)rand()%255;
		ag=(ice_sint)rand()%255;
		ab=(ice_sint)rand()%255;
		//aa=(ice_sint)rand()%255;
		
		ice_video_texture_triangle_draw(
			0, texture_1,
			ax, ay, az,
			bx, by, bz,
			cx, cy, cz,
			0, 0,
			0, 255,
			255, 255,
			ar, ag, ab, aa,
			ar, ag, ab, aa,
			ar, ag, ab, aa
		);
	}
	
	ice_real end=ice_clock_get();
	
	ice_video_buffer();
	getch();
	
	ice_deinit();

	printf(
		"Drawing %u triangles took %f seconds\n",
		draws,
		end-start
	);
	
	return 0;
}