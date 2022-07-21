#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Cache options
   Option: Runtime acqurie flag */
#define CACHE_CONFIG_ACQUIRE

/* Option: runtime check */
// #define CACHE_CONFIG_RUNTIME_CHECK 

/* Option: Log req stats */
#define CACHE_LOG_REQ

// Requests
struct cache_req {
    uint64_t tag;
    union {
        uint64_t tag2;
        struct {
            uint64_t _unused : 56; // implies cache line <= 2^55 B
            uint8_t type     : 4; 
            uint8_t cache_id : 4; 
        };
    };
};

/* token interface */
// DONE: hold pointer ?
// DONE: use tag as verification key
typedef struct cache_token_t {
    struct {
        uint64_t tag: 48; // addr = tag + line_ofst
        uint16_t line_ofst;
    };

    struct {
        uint64_t head_addr: 48;
        // uint8_t ver;
        uint16_t cache;
    };
} cache_token_t;

/* cache line header */
typedef struct line_header {
    struct {
        uint64_t tag: 48;
        uint16_t acq_count;
    };
    union {
        uint64_t line_meta;
        struct {
            uint32_t slot;
            uint32_t weight; 
            // uint8_t version;
        };
    };

    // compatible with shenango atomic
    int status;
    int flags;
} line_header;


/* Cache REQ spec */
enum {
    CACHE_REQ_WRITE = 0,
    CACHE_REQ_READ,
    CACHE_REQ_EVICT,
    CACHE_REQ_MEMCOPY,
    CACHE_REQ_MEMMOVE
};

#define CACHE_REQ_META (8)
#define CACHE_LINE_LIMIT (1 << 10)
#define REQ_META_MASK (((uint64_t)1 << (64-CACHE_REQ_META)) - 1)

#define CACHE_REQ_INFLIGHT 64
#ifndef MEMMAP_CACHE_REQ 
    #define MEMMAP_CACHE_REQ (sbuf)
#endif


/* Cache spec */
typedef unsigned cache_t;
// TODO: Constant Propogation?
#define OPT_NUM_CACHE 16

#ifndef __cache_access_handler 
    #define __cache_access_handler _cache_access_groupassoc
#endif
#ifndef __cache_select 
    #define __cache_select _cache_select_groupassoc_lru
#endif


/* Cache Line Spec */
// line flags
enum {
    LINE_FLAGS_DIRTY   = 1 << 0,
    LINE_FLAGS_RACCESS = 1 << 1,
    // LINE_FLAGS_ACQUIRE = 1 << 2,
};

// line status
enum line_status {
    LINE_IDLE=0, // IDLE only happens on init
    LINE_ALLOC,
    LINE_READY,
    LINE_SYNC,
    LINE_END
};

/* 
High-level interfaces 
*/

// init, should be called only after common init
void cache_init();

// create
cache_t cache_create(unsigned size, unsigned linesize, void * linebase);

// Access Level, token interface
void cache_acquire(cache_t cache, intptr_t addr, size_t nitems, size_t size, cache_token_t *tokens);
void cache_release(cache_token_t *tokens, int cnt);
void cache_re_acquire(cache_token_t *token);

// TODO: consider inline
// TODO: fixed base?
void cache_request(cache_t cache, intptr_t addr, cache_token_t *token);

void cache_sync(cache_token_t *token);
void cache_await(cache_token_t *token);

void * cache_access(cache_token_t *token);
void * cache_access_mut(cache_token_t *token);
void * cache_access_nrtc(cache_token_t *token);
void * cache_access_nrtc_mut(cache_token_t *token);

// if version missmatch, request new one
void cache_access_check(cache_token_t *token);
void cache_evict(cache_token_t *token, intptr_t addr);

typedef struct cache_stats {
    cache_t cache_id;
    uint64_t total_reqs;
    uint64_t miss_reqs;
    
    uint64_t total_awaits;
    uint64_t early_awaits;
} cache_stats;
int get_cache_logs(cache_stats *cs);

#ifdef __cplusplus
}
#endif
#endif
