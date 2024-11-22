#ifndef ICE_H
#define ICE_H

#define ICE_VERSION_MAJOR 1
#define ICE_VERSION_MINOR 5
#define ICE_VERSION_PATCH 2

typedef unsigned char ice_char;
typedef unsigned int  ice_uint;
typedef signed int    ice_sint;
typedef float         ice_real;

void ice_log(
	ice_char *message
);

void ice_init();

void ice_deinit();

ice_uint ice_update(
	ice_real tick
);

/***********************************[CLOCK]***********************************/

ice_uint ice_clock_init();

void ice_clock_deinit();

ice_real ice_clock_get();

/***********************************[VIDEO]***********************************/

#define ICE_VIDEO_EFFECT_DEFAULT 0
#define ICE_VIDEO_EFFECT_CULLING 1
#define ICE_VIDEO_EFFECT_LATTICE 2
#define ICE_VIDEO_EFFECT_AMBIENT 4
#define ICE_VIDEO_EFFECT_REFLECT 8

#define ICE_VIDEO_CLEAR_COLOR 1
#define ICE_VIDEO_CLEAR_DEPTH 2

ice_uint ice_video_init();

void ice_video_deinit();

void ice_video_update();

ice_real ice_video_width_get();

ice_real ice_video_height_get();

void ice_video_clear(
	ice_uint attribute
);

void ice_video_scissor_set(
	ice_real ax,
	ice_real ay,
	ice_real bx,
	ice_real by
);

void ice_video_rectangle_draw(
	ice_uint texture_id,
	ice_real d_ax,
	ice_real d_ay,
	ice_real d_bx,
	ice_real d_by,
	ice_real s_ax,
	ice_real s_ay,
	ice_real s_bx,
	ice_real s_by,
	ice_real c_r,
	ice_real c_g,
	ice_real c_b,
	ice_real c_a
);

void ice_video_triangle_draw(
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
	ice_real c_r,
	ice_real c_g,
	ice_real c_b,
	ice_real c_a
);

ice_uint ice_video_texture_load(
	ice_uint asset_id
);

void ice_video_texture_delete(
	ice_uint texture_id
);

void ice_video_texture_flush();

ice_uint ice_video_texture_asset_id_get(
	ice_uint texture_id
);

ice_real ice_video_texture_width_get(
	ice_uint texture_id
);

ice_real ice_video_texture_height_get(
	ice_uint texture_id
);

ice_real ice_video_texture_length_get(
	ice_uint texture_id
);

ice_real ice_video_texture_seek_get(
	ice_uint texture_id
);

void ice_video_texture_seek_set(
	ice_uint texture_id,
	ice_real position
);

ice_uint ice_video_array_new(
	ice_uint size
);

void ice_video_array_delete(
	ice_uint array_id
);

void ice_video_array_flush();

ice_uint ice_video_array_size_get(
	ice_uint array_id
);

void ice_video_array_set(
	ice_uint array_id,
	ice_uint index,
	ice_real value
);

ice_real ice_video_array_get(
	ice_uint array_id,
	ice_uint index
);

// ice_uint ice_video_model_new();

ice_uint ice_video_model_load(
	ice_uint asset_id,
	ice_uint mesh_id
);

void ice_video_model_delete(
	ice_uint model_id
);

void ice_video_model_flush();

void ice_video_model_draw(
	ice_uint model_id,
	ice_uint texture_id,
	ice_uint pose_matrix_id,
	ice_uint modelview_matrix_id,
	ice_uint projection_matrix_id,
	ice_uint texture_matrix_id,
	ice_uint effect,
	ice_real c_r,
	ice_real c_g,
	ice_real c_b,
	ice_real c_a
);

/***********************************[AUDIO]***********************************/

#define ICE_AUDIO_STATE_NONE    0
#define ICE_AUDIO_STATE_PAUSED  1
#define ICE_AUDIO_STATE_PLAYING 2
#define ICE_AUDIO_STATE_LOOPING 3

ice_uint ice_audio_init();

void ice_audio_deinit();

void ice_audio_update();

ice_uint ice_audio_sample_load(
	ice_uint file_id
);

void ice_audio_sample_delete(
	ice_uint sample_id
);

void ice_audio_sample_flush();

ice_real ice_audio_sample_length_get(
	ice_uint sample_id
);

ice_uint ice_audio_source_new();

void ice_audio_source_delete(
	ice_uint source_id
);

void ice_audio_source_flush();

ice_uint ice_audio_source_sample_get(
	ice_uint source_id
);

void ice_audio_source_sample_set(
	ice_uint source_id,
	ice_uint sample_id
);

ice_real ice_audio_source_seek_get(
	ice_uint source_Id
);

void ice_audio_source_seek_set(
	ice_uint source_id,
	ice_real position
);

ice_real ice_audio_source_volume_get(
	ice_uint source_id
);

void ice_audio_source_volume_set(
	ice_uint source_id,
	ice_real volume
);

ice_uint ice_audio_source_state_get(
	ice_uint source_id
);

void ice_audio_source_state_set(
	ice_uint source_id,
	ice_uint state
);

ice_uint ice_audio_stream_load(
	ice_uint file_id
);

void ice_audio_stream_delete(
	ice_uint stream_id
);

void ice_audio_stream_flush();

ice_real ice_audio_stream_length_get(
	ice_uint stream_id
);

ice_real ice_audio_stream_seek_get(
	ice_uint stream_id
);

void ice_audio_stream_seek_set(
	ice_uint stream_id,
	ice_real position
);

ice_real ice_audio_stream_volume_get(
	ice_uint stream_id
);

void ice_audio_stream_volume_set(
	ice_uint stream_id,
	ice_real volume
);

ice_uint ice_audio_stream_state_get(
	ice_uint stream_id
);

void ice_audio_stream_state_set(
	ice_uint stream_id,
	ice_uint state
);

/***********************************[INPUT]***********************************/

#define ICE_INPUT_DEVICE_KEYBOARD 0
#define ICE_INPUT_DEVICE_MOUSE    1
#define ICE_INPUT_DEVICE_GAMEPAD  2

#define ICE_INPUT_SENSOR_KEYBOARD_BUTTON 0

#define ICE_INPUT_SENSOR_MOUSE_OFFSET_X      0
#define ICE_INPUT_SENSOR_MOUSE_OFFSET_Y      1
#define ICE_INPUT_SENSOR_MOUSE_LEFT_BUTTON   2
#define ICE_INPUT_SENSOR_MOUSE_RIGHT_BUTTON  3
#define ICE_INPUT_SENSOR_MOUSE_MIDDLE_BUTTON 4

ice_uint ice_input_init();

void ice_input_deinit();

void ice_input_update();

ice_char ice_input_poll(
	ice_uint *device_id,
	ice_uint *sensor_id,
	ice_real *sensor_state
);

ice_real ice_input_get(
	ice_uint device_id,
	ice_uint sensor_id
);

/***********************************[CACHE]***********************************/

ice_uint ice_cache_init();

void ice_cache_deinit();

ice_uint ice_cache_slot_open(
	ice_uint slot_id
);

void ice_cache_slot_close(
	ice_uint slot_id
);

void ice_cache_slot_delete(
	ice_uint slot_id
);

void ice_cache_slot_seek(
	ice_uint slot_id,
	ice_uint address
);

ice_char ice_cache_slot_read(
	ice_uint slot_id
);

void ice_cache_slot_write(
	ice_uint slot_id,
	ice_char byte
);

#endif