/* important note: deadlock could happen if acquire is enabled */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

/* cache policy funcitons */
static inline void check_ok(int b, const char * s) {
	if (!b) {
		fprintf(stderr, "ASSERT ERROR: %s.\n", s);
		exit(-1);
	}
}

static inline unsigned rand_next() {
	const int mult = 22695477;
	const int inc = 1;
	static unsigned cur = 2333;
	return (cur = cur * mult + inc);
}

/* Group assoc */
const int group_bits = 2;
const uint64_t groups = 1 << group_bits;
const uint64_t group_mask = ~(groups - 1);

// checkers
static inline  void _cache_access_check(cache_token_t *token, int mut) {
	// mark line as dirty
	if (mut)
		token_set_flag(token,LINE_FLAGS_DIRTY);
}

// check acquire
static inline int _cache_eviction_check(cache_t cache, unsigned slot) {
	int error = 0;
#ifdef CACHE_CONFIG_ACQUIRE
	error |= (cache_header_field(cache,slot,acq_count) != 0);
#endif
	return error;
}

// valid token before calling this func
static inline void _cache_access_groupassoc(cache_token_t *token, int mut) {
	// mark line as dirty
	_cache_access_check(token, mut);
	// mark self as assoc
	unsigned base = token_header_field(token,slot) & group_mask;

	for (int i = 0; i < groups; i++) {
		cache_clear_flag(token->cache,base+i,LINE_FLAGS_RACCESS);
	}
	token_set_flag(token, LINE_FLAGS_RACCESS);
}

/* direct assoc: XXX: will not respect eviction checks in direct mode */
static inline void _cache_select_directassoc(cache_t cache, uint64_t tag, cache_token_t *token) {
	unsigned linesize = cache_get_field(cache,linesize);
	unsigned slot = (tag / linesize) & (cache_get_field(cache,slots) - 1);
	token->head_addr = (uint64_t) cache_get_line(cache, slot);
}

static inline void _cache_select_groupassoc_rand(cache_t cache, uint64_t tag, cache_token_t *token) {
	unsigned linesize = cache_get_field(cache,linesize);
	unsigned base = (tag/linesize/groups) & (cache_get_field(cache,slots)/groups - 1);
	base <<= group_bits;
	// check if in the group already
	for (int i = 0; i < groups; i++) {
		if (cache_header_field(cache,base+i,tag) == tag) {
			token->head_addr = (uint64_t) cache_get_line(cache,base+i);
			return;
		}
	}
	// check idle
	for (int i = 0; i < groups; i++) {
		if (cache_header_field(cache,base+i,status) == LINE_IDLE) {
			token->head_addr = (uint64_t) cache_get_line(cache,base+i);
			return;
		}
	}
	// random select a single element to evict
	unsigned slot = base + (rand_next() & (groups - 1));
	token->head_addr = (uint64_t) cache_get_line(cache,slot);
	check_ok(_cache_eviction_check(cache, slot) == 0, "cache victim wrong");
}

static inline void _cache_select_groupassoc_lru(cache_t cache, uint64_t tag, cache_token_t *token) {
	// try group assoc
	unsigned linesize = cache_get_field(cache,linesize);
	unsigned base = (tag/linesize/groups) & (cache_get_field(cache,slots)/groups - 1);
	base <<= group_bits; 
	int flag = 0;
	// check if is in the group
	for (int i = 0; i < groups; i++) {
		if (cache_header_field(cache,base+i,tag) == tag) {
			token->head_addr = (uint64_t) cache_get_line(cache,base+i);
			return;
		}
	}

	// not in the group, find 1. idle 2. lru
	for (int i = 0; i < groups; i++) {
		if (cache_header_field(cache,base+i,status) == LINE_IDLE) {
			token->head_addr = (uint64_t) cache_get_line(cache,base+i);
			return;
		}
		if (!flag && !cache_check_flag(cache,base+i,LINE_FLAGS_RACCESS) &&
				_cache_eviction_check(cache, base + i) == 0) {
			flag = 1;
			token->head_addr = (uint64_t) cache_get_line(cache,base+i);
		}
	}

	check_ok(flag, "cache out of space");
}
