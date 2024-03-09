#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "ice.h"

void print_graphics(
	ice_uint d_texture_id,
	ice_uint font_texture_id,
	ice_sint font_texture_width,
	ice_sint font_texture_height,
	ice_sint x,
	ice_sint y,
	ice_char *text,
	ice_char c_ar,
	ice_char c_ag,
	ice_char c_ab,
	ice_char c_aa
) {
	if (text==NULL) {
		return;
	}
	
	ice_uint char_index = 0;
	ice_sint ch         = (ice_sint)text[char_index];
	
	ice_sint lookup_x;
	ice_sint lookup_y;
	
	while (ch!='\0') {
		ch-=32;
		
		lookup_x = (ch%16)*font_texture_width;
		lookup_y = (ch/16)*font_texture_height;
		
		ice_video_texture_rectangle_draw(
			d_texture_id,
			font_texture_id,
			x, y,
			x+font_texture_width-1,
			y+font_texture_height-1,
			lookup_x,
			lookup_y,
			lookup_x+font_texture_width-1,
			lookup_y+font_texture_height-1,
			c_ar, c_ag, c_ab, c_aa
		);
		
		char_index++;
		ch=(ice_sint)text[char_index];
		x+=font_texture_width;
	}
}

void ice_init() {
	ice_uint font   = ice_video_texture_load(1);
	ice_uint sample = ice_audio_sample_load(0);
	ice_uint source = ice_audio_source_new();
	
	ice_audio_source_sample_set(
		source,
		sample
	);
	
	ice_audio_source_state_set(
		source,
		STATE_PLAYING
	);
	
	print_graphics(
		0, font,
		12, 16,
		57, 91,
		(ice_char *)"Press ESC to exit",
		255, 255, 255, 255
	);
}

void ice_deinit() {}

ice_uint ice_update(
	ice_real tick
) {
	return ice_input_get(0,0x01)==1.0f;
}