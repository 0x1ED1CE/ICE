ice_uint ice_audio_init() {
    return 0;
}

void ice_audio_deinit() {
}

void ice_audio_buffer() {
}

void ice_audio_sample_flush() {
}

void ice_audio_source_flush() {
}

ice_uint ice_audio_sample_load(
	ice_uint file_id
) {
    return 0;
}

void ice_audio_sample_delete(
	ice_uint sample_id
) {
}

ice_real ice_audio_sample_length_get(
	ice_uint sample_id
) {
    return 0;
}

ice_uint ice_audio_source_new() {
	return 0;
}

void ice_audio_source_delete(
	ice_uint source_id
) {
}

ice_uint ice_audio_source_sample_get(
	ice_uint source_id
) {
    return 0;
}

void ice_audio_source_sample_set(
	ice_uint source_id,
	ice_uint sample_id
) {
}

ice_real ice_audio_source_position_get(
	ice_uint source_Id
) {
    return 0;
}

void ice_audio_source_position_set(
	ice_uint source_id,
	ice_real position
) {
}

ice_char ice_audio_source_state_get(
	ice_uint source_id
) {
    return 0;
}

void ice_audio_source_state_set(
	ice_uint source_id,
	ice_char state
) {
}

ice_real ice_audio_source_volume_get(
	ice_uint source_id
) {
    return 0;
}

void ice_audio_source_volume_set(
	ice_uint source_id,
	ice_real volume
) {
}