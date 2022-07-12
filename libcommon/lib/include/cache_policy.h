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
        fprintf(stderr, "ASSERT ERROR: %s.", s);
        exit(-1);
    }
}

/* Group assoc */
static inline unsigned rand_next() {
    const int mult = 22695477;
    const int inc = 1;
    static unsigned cur = 2333;
    return (cur = cur * mult + inc);
}
const int group_bits = 2;
const int groups = 1 << group_bits;

// checkers
static void inline _cache_access_check(cache_token_t) {
    // mark line as dirty
    if (mut)
        token_set_flag(token, CACHE_FLAGS_DIRTY);
}

static int inline _cache_eviction_check(cache_token_t token) {
    int error = 0;
#ifdef CACHE_CONFIG_RUNTIME_CHECK
    error |= (token_get_meta(token,version) != token.ver);
#endif
#ifdef CACHE_CONFIG_ACQUIRE
    error |= token_check_flag(t[i], CACHE_FLAGS_ACQUIRE);
#endif
    return error;
}

static void inline _cache_access_groupassoc(cache_token_t token, int mut) {
    // mark line as dirty
    _cache_access_check(token, mut);
    // mark self as assoc
    unsigned base = token.slot & ~(groups-1);
    for (int i = 0; i < groups; i++) {
        cache_token_t tk = (cache_token_t){.cache=token.cache, .slot=base+i};
        token_reset_flag(tk,CACHE_FLAGS_RACCESS);
    }
    token_set_flag(token,CACHE_FLAGS_RACCESS);
}

/* direct assoc: XXX: will not respect eviction checks in direct mode */
static inline cache_token_t _cache_select_directassoc(cache_t cache, uint64_t tag) {
    cache_token_t token;
    token.cache = cache;
    unsigned linesize = cache_get(cache,linesize);
    token.slot = (tag / linesize) & (cache_get(cache,size)/linesize - 1);
    return token;
}

static inline cache_token_t _cache_select_groupassoc_rand(cache_t cache, uint64_t tag) {
    unsigned linesize = cache_get(cache,linesize);
    unsigned base = (tag/linesize/groups) & (cache_get(cache,size)/linesize/groups - 1);
    base <<= group_bits;
    cache_token_t t[groups];
    for (int i = 0; i < groups; i++) {
        t[i] = (cache_token_t){.cache=cache, .slot=base+i};
        if (token_get_meta(t[i],status) == CACHE_IDLE)
            return t[i];
    }
    // random select a single element to evict
    cache_token_t victim = t[rand_next() % groups];
    check_ok(_cache_eviction_check(victim) == 0, "cache victim wrong");
    return victim;
}

static inline cache_token_t _cache_select_groupassoc_lru(cache_t cache, uint64_t tag) {

    // try group assoc
    unsigned linesize = cache_get(cache,linesize);
    unsigned base = (tag/linesize/groups) & (cache_get(cache,size)/linesize/groups - 1);
    int flag = 0;
    base <<= group_bits;
    cache_token_t t, tlru;
    t.cache = cache;
    // check if is in the group
    for (int i = 0; i < groups; i++) {
        t.slot = base + i;
        if (cache_get_meta(cache, t, tag) == tag) return t;
    }
    
    // not in the group, find 1. idle 2. lru
    for (int i = 0; i < groups; i++) {
        t.slot = base+i;
        if (token_get_meta(t,status) == CACHE_IDLE)
            return t;
        if (!token_check_flag(t,CACHE_FLAGS_RACCESS) &&
                _cache_eviction_check(t) == 0)
            flag = 1;
            tlru = t;
        }
    }

    check_ok(flag, "cache out of space");
    return tlru;
}
