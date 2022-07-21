#ifndef _CACHE_INTERNAL_H_
#define _CACHE_INTERNAL_H_
#include "cache.h"
#include <stdint.h>

static struct cache_internal {
    // pointers
    // meta stay with cache lines
    // | header | data ... |
    char * linebase;
    // linesizes
    unsigned linesize;
    unsigned size;
    unsigned slots;

    uint64_t total_reqs;
    uint64_t miss_reqs;

    uint64_t total_awaits;
    uint64_t early_awaits;
} caches[OPT_NUM_CACHE];

static int cache_cnt = 0;

static inline uint64_t cache_tag_mask(uint64_t linesize, intptr_t addr) {
    return ((uint64_t)addr & (linesize - 1));
}

static inline uint64_t cache_ofst_mask(uint64_t linesize, intptr_t addr) {
    return ((uint64_t)addr & ~(linesize - 1));
}

/* Cache Line Header Inlines */
#define header_get_field(h_ptr,field) \
    ((h_ptr)->field)
#define header_set_flag(h_ptr,flag) \
    ((h_ptr)->flags |= (flag))
#define header_clear_flag(h_ptr,flag) \
    ((h_ptr)->flags &= (~(flag)))
#define header_check_flag(h_ptr,flag) \
    ((h_ptr)->flags & (flag))

/* Token Inlines/Macros */
#define token_get_header(t_ptr) \
    ((struct line_header *)(uintptr_t)((t_ptr)->head_addr))
#define token_get_data(t_ptr) \
    ((void *) ((char *)(uintptr_t)((t_ptr)->head_addr)+sizeof(struct line_header)+(t_ptr->line_ofst)))
#define token_set_flag(t_ptr,flag) \
    (header_set_flag(token_get_header(t_ptr), flag))
#define token_clear_flag(t_ptr,flag) \
    (header_clear_flag(token_get_header(t_ptr), flag))
#define token_check_flag(t_ptr,flag) \
    (header_check_flag(token_get_header(t_ptr), flag))
#define token_header_field(t_ptr,field) \
    (header_get_field(token_get_header(t_ptr),field))

/* Cache Inlines/Macros */
#define cache_get_field(cache,field) \
    (caches[cache].field)
#define cache_get_line(cache,slot) \
    (caches[cache].linebase+(slot)*(sizeof(line_header)+cache_get_field(cache,linesize)))
#define cache_get_header(cache,slot) \
    ((struct line_header *)cache_get_line(cache,slot))
#define cache_get_ldata(cache,slot) \
    (cache_get_line(cache,slot)+sizeof(line_header))
#define cache_header_field(cache,slot,field) \
    (header_get_field(cache_get_header(cache,slot),field))

#define cache_set_flag(cache,slot,flag) \
    header_set_flag(cache_get_header(cache,slot), flag)
#define cache_clear_flag(cache,slot,flag) \
    header_clear_flag(cache_get_header(cache,slot), flag)
#define cache_check_flag(cache,slot,flag) \
    header_check_flag(cache_get_header(cache,slot), flag)

#endif
