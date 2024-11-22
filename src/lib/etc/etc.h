// Extended ICE functions

void ice_video_text_draw(
	char *message,
	ice_uint texture_id,
	ice_real d_x,
	ice_real d_y,
	ice_real c_w,
	ice_real c_h,
	ice_uint c_c,
	ice_real r,
	ice_real g,
	ice_real b,
	ice_real a
) {
	ice_real t_w = ice_video_texture_width_get(texture_id);
	ice_real t_h = ice_video_texture_height_get(texture_id);
	ice_real b_w = ice_video_width_get();
	ice_real b_h = ice_video_height_get();
	ice_real s_w = c_w/t_w;
	ice_real s_h = c_h/t_h;
	ice_real d_w = c_w/b_w;
	ice_real d_h = c_h/b_h;
	ice_real o_x = d_x;
	ice_real o_y = d_y;

	for (ice_uint i=0; message[i]; i++) {
		ice_uint code = (ice_uint)message[i];
		ice_real s_x  = (ice_real)((code-33)%c_c)*s_w;
		ice_real s_y  = (ice_real)((code-33)/c_c)*s_h;

		if (code>32) {
			ice_video_rectangle_draw(
				texture_id,
				o_x,o_y,
				o_x+d_w,
				o_y+d_h,
				s_x,s_y,
				s_x+s_w,
				s_y+s_h,
				r,g,b,a
			);

			o_x += d_w;
		} else if (code=='\n') {
			o_x  = d_x;
			o_y += d_h;
		} else if (code=='\t') {
			o_x += d_w*4;
		} else {
			o_x += d_w;
		}
	}
}