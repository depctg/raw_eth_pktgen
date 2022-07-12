#ifndef _CACHE_INTERNAL_H_
#define _CACHE_INTERNAL_H_

static struct cache_internal {
    // pointers
    struct cache_meta * metabase;
    char * linebase;
    // linesizes
    unsigned linesize;
    unsigned size;
    unsigned slots;
} caches[OPT_NUM_CACHE];

static int cache_cnt = 0;

enum cache_status {
    // IDLE only happens on init
    CACHE_IDLE=0,
    CACHE_ALLOC,
    CACHE_READY,
    CACHE_SYNC,
    CACHE_END
};

// impl macros

#define cache_get(cache,field) \
    ((caches[cache]).field)
#define cache_get_meta(cache,token,field) \
    ((caches[cache]).metabase[cache_token_slot(token)].field)
#define cache_get_line(cache,token) \
    ((caches[cache]).linebase+cache_token_slot(token)*caches[cache].linesize)
#define cache_set_flag(cache,token,flag) \
    (((caches[cache]).metabase[cache_token_slot(token)].flags) |= flag)
#define cache_reset_flag(cache,token,flag) \
    (((caches[cache]).metabase[cache_token_slot(token)].flags) &= (~(flag)))
#define cache_check_flag(cache,token,flag) \
    (((caches[cache]).metabase[cache_token_slot(token)].flags) & (flag))

#define token_get_cache(token,field) \
    ((caches[token.cache]).field)
#define token_get_meta(token,field) \
    cache_get_meta(token.cache,token,field)
#define token_get_line(token) \
    cache_get_line(token.cache,token)
#define token_set_flag(token,flag) \
    cache_set_flag(token.cache,token,flag)
#define token_reset_flag(token,flag) \
    cache_reset_flag(token.cache,token,flag)
#define token_check_flag(token,flag) \
    cache_check_flag(token.cache,token,flag)

/* Utils */

// is power of 2, non-zero
static inline int is_pow2(unsigned v) {
        return v && ((v & (v - 1)) == 0);
}

static inline uint64_t align_with_pow2(uint64_t x) {
    if (is_pow2(x)) return x;
    int nlz = __builtin_clzll(x);
    return ((uint64_t)1 << (64 - nlz));
}

static inline uint64_t cache_tag_mask(uint64_t linesize, intptr_t addr) {
    return ((uint64_t)addr & (linesize - 1));
}

// align addr to cache line size
static inline uint64_t align_next_free(uint64_t addr, size_t ds, uint64_t cls) {
    if ((cls - (addr % cls)) >= ds) return addr;
    // else align to next cls
    return addr + cls - (addr % cls);
}


#endif
