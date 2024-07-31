#ifndef ICE_H
#define ICE_H

#define ICE_VERSION 13

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

#define ICE_VIDEO_ARRAY_GENERIC_1  0x00
#define ICE_VIDEO_ARRAY_MATRIX_16  0x1F
#define ICE_VIDEO_ARRAY_POSITION_3 0x22
#define ICE_VIDEO_ARRAY_TEXTURE_2  0x31
#define ICE_VIDEO_ARRAY_NORMAL_3   0x42
#define ICE_VIDEO_ARRAY_MODEL_1    0x50
#define ICE_VIDEO_ARRAY_MODEL_2    0x51
#define ICE_VIDEO_ARRAY_MODEL_3    0x52

#define ICE_VIDEO_CLEAR_COLOR 0
#define ICE_VIDEO_CLEAR_DEPTH 1

ice_uint ice_video_init();

void ice_video_deinit();

void ice_video_update();

void ice_video_clear(
	ice_uint attribute
);

void ice_video_texture_flush();

void ice_video_stream_flush();

void ice_video_array_flush();

ice_uint ice_video_width_get();

ice_uint ice_video_height_get();

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
);

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
);

ice_uint ice_video_array_new(
	ice_uint type,
	ice_uint size
);

void ice_video_array_delete(
	ice_uint array_id
);

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

ice_uint ice_video_model_load(
	ice_uint file_id
);

void ice_video_model_draw(
	ice_uint model_array_id,
	ice_uint projection_array_id,
	ice_real color_r,
	ice_real color_g,
	ice_real color_b,
	ice_real color_a
);

ice_uint ice_video_stream_load(
	ice_uint file_id
);

void ice_video_stream_delete(
	ice_uint stream_id
);

void ice_video_stream_draw(
	ice_uint stream_id
);

ice_real ice_video_stream_length_get(
	ice_uint stream_id
);

ice_real ice_video_stream_width_get(
	ice_uint stream_id
);

ice_real ice_video_stream_height_get(
	ice_uint stream_id
);

ice_real ice_video_stream_position_get(
	ice_uint stream_id
);

void ice_video_stream_position_set(
	ice_uint stream_id,
	ice_real position
);

/***********************************[AUDIO]***********************************/

#define ICE_AUDIO_STATE_NONE    0
#define ICE_AUDIO_STATE_PAUSED  1
#define ICE_AUDIO_STATE_PLAYING 2
#define ICE_AUDIO_STATE_LOOPING 3

ice_uint ice_audio_init();

void ice_audio_deinit();

void ice_audio_update();

void ice_audio_sample_flush();

void ice_audio_source_flush();

void ice_audio_stream_flush();

ice_uint ice_audio_sample_load(
	ice_uint file_id
);

void ice_audio_sample_delete(
	ice_uint sample_id
);

ice_real ice_audio_sample_length_get(
	ice_uint sample_id
);

ice_uint ice_audio_source_new();

void ice_audio_source_delete(
	ice_uint source_id
);

ice_uint ice_audio_source_sample_get(
	ice_uint source_id
);

void ice_audio_source_sample_set(
	ice_uint source_id,
	ice_uint sample_id
);

ice_real ice_audio_source_position_get(
	ice_uint source_Id
);

void ice_audio_source_position_set(
	ice_uint source_id,
	ice_real position
);

ice_uint ice_audio_source_state_get(
	ice_uint source_id
);

void ice_audio_source_state_set(
	ice_uint source_id,
	ice_uint state
);

ice_real ice_audio_source_volume_get(
	ice_uint source_id
);

void ice_audio_source_volume_set(
	ice_uint source_id,
	ice_real volume
);

ice_uint ice_audio_stream_load(
	ice_uint file_id
);

void ice_audio_stream_delete(
	ice_uint stream_id
);

ice_real ice_audio_stream_length_get(
	ice_uint stream_id
);

ice_real ice_audio_stream_position_get(
	ice_uint stream_id
);

void ice_audio_stream_position_set(
	ice_uint stream_id,
	ice_real position
);

ice_uint ice_audio_stream_state_get(
	ice_uint stream_id
);

void ice_audio_stream_state_set(
	ice_uint stream_id,
	ice_uint state
);

ice_real ice_audio_stream_volume_get(
	ice_uint stream_id
);

void ice_audio_stream_volume_set(
	ice_uint stream_id,
	ice_real volume
);

/***********************************[INPUT]***********************************/

#define ICE_INPUT_DEVICE_KEYBOARD 0
#define ICE_INPUT_DEVICE_MOUSE    1
#define ICE_INPUT_DEVICE_GAMEPAD  2

#define ICE_INPUT_SENSOR_KEYBOARD_BUTTON 0

#define ICE_INPUT_SENSOR_MOUSE_POSITION_X    0
#define ICE_INPUT_SENSOR_MOUSE_POSITION_Y    1
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
