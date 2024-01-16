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
	ice_sint d_ax,
	ice_sint d_ay,
	ice_char c_ar,
	ice_char c_ag,
	ice_char c_ab,
	ice_char c_aa
);

void ice_video_texture_rectangle_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_sint d_ax,
	ice_sint d_ay,
	ice_sint d_bx,
	ice_sint d_by,
	ice_sint s_ax,
	ice_sint s_ay,
	ice_sint s_bx,
	ice_sint s_by,
	ice_char c_ar,
	ice_char c_ag,
	ice_char c_ab,
	ice_char c_aa
);

void ice_video_texture_triangle_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_sint d_ax,
	ice_sint d_ay,
	ice_sint d_az,
	ice_sint d_bx,
	ice_sint d_by,
	ice_sint d_bz,
	ice_sint d_cx,
	ice_sint d_cy,
	ice_sint d_cz,
	ice_sint s_ax,
	ice_sint s_ay,
	ice_sint s_bx,
	ice_sint s_by,
	ice_sint s_cx,
	ice_sint s_cy,
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
);

ice_uint ice_video_vertex_new(
	ice_uint size
);

void ice_video_vertex_delete(
	ice_uint vertex_id
);

ice_uint ice_video_vertex_size_get(
	ice_uint vertex_id
);

void ice_video_vertex_set_uint(
	ice_uint vertex_id,
	ice_uint index,
	ice_uint value
);

ice_uint ice_video_vertex_get_uint(
	ice_uint vertex_id,
	ice_uint index
);

void ice_video_vertex_set_sint(
	ice_uint vertex_id,
	ice_uint index,
	ice_sint value
);

ice_sint ice_video_vertex_get_sint(
	ice_uint vertex_id,
	ice_uint index
);

void ice_video_vertex_set_real(
	ice_uint vertex_id,
	ice_uint index,
	ice_real value
);

ice_real ice_video_vertex_get_real(
	ice_uint vertex_id,
	ice_uint index
);

void ice_video_vertex_texture_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
	ice_real render_matrix[4][4],
	ice_uint position_vertex_id,
	ice_uint texture_vertex_id,
	ice_uint face_vertex_id
);

void ice_video_texture_flush();

void ice_video_vertex_flush();

void ice_video_buffer();

ice_uint ice_video_init();

void ice_video_deinit();

#endif