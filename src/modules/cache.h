#ifndef ICE_CACHE_H
#define ICE_CACHE_H

ice_uint ice_cache_slot_open(
	ice_uint slot_id
);

void ice_cache_slot_close(
	ice_uint slot_id
);

void ice_cache_slot_delete(
	ice_uint slot_id
);

ice_uint ice_cache_init();

void ice_cache_deinit();

#endif