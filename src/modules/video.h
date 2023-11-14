#ifndef ICE_VIDEO_H
#define ICE_VIDEO_H

ice_uint ice_video_texture_new(
	ice_uint width,
	ice_uint height,
	ice_char z_enable
);

ice_uint ice_video_texture_load(
	ice_uint file_id
);

void ice_video_texture_delete(
	ice_uint texture_id
);

ice_uint ice_video_texture_width_get(
	ice_uint texture_id
);

ice_uint ice_video_texture_height_get(
	ice_uint texture_id
);

void ice_video_texture_clear(
	ice_uint texture_id
);

void ice_video_texture_pixel_draw(
	ice_uint d_texture_id,
	ice_float d_ax,
	ice_float d_ay,
	ice_float c_ar,
	ice_float c_ag,
	ice_float c_ab,
	ice_float c_aa
);

void ice_video_texture_rectangle_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_float d_ax,
	ice_float d_ay,
	ice_float d_bx,
	ice_float d_by,
	ice_float s_ax,
	ice_float s_ay,
	ice_float s_bx,
	ice_float s_by,
	ice_float c_ar,
	ice_float c_ag,
	ice_float c_ab,
	ice_float c_aa
);

void ice_video_texture_triangle_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_float d_ax,
	ice_float d_ay,
	ice_float d_az,
	ice_float d_bx,
	ice_float d_by,
	ice_float d_bz,
	ice_float d_cx,
	ice_float d_cy,
	ice_float d_cz,
	ice_float s_ax,
	ice_float s_ay,
	ice_float s_bx,
	ice_float s_by,
	ice_float s_cx,
	ice_float s_cy,
	ice_float c_ar,
	ice_float c_ag,
	ice_float c_ab,
	ice_float c_aa,
	ice_float c_br,
	ice_float c_bg,
	ice_float c_bb,
	ice_float c_ba,
	ice_float c_cr,
	ice_float c_cg,
	ice_float c_cb,
	ice_float c_ca
);

void ice_video_flush();

void ice_video_buffer();

ice_uint ice_video_init();

void ice_video_deinit();

#endif