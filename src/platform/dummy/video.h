ice_uint ice_video_texture_new(
	ice_uint width,
	ice_uint height,
	ice_uint attributes
) {
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

void ice_video_texture_clear(
	ice_uint texture_id
) {
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

void ice_video_array_model_draw(
	ice_uint d_texture_id,
	ice_uint s_texture_id,
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
	return 0;
}

void ice_video_deinit() {
}