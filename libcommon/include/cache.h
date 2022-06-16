#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

enum {
    CACHE_REQ_WRITE = 0,
    CACHE_REQ_READ,
    CACHE_REQ_EVICT,
    CACHE_REQ_MEMCOPY,
    CACHE_REQ_MEMMOVE
};

#define CACHE_TAG_ALIGN (4)
#define CACHE_TAG_MASK (~(((uint64_t)(1 << CACHE_TAG_ALIGN))-1))

// Requests
struct cache_req {
    uint64_t tag;
    union {
        uint64_t newtag;
        struct {
            uint8_t type     : 4; // implies cache line >= 16B
            uint64_t _unused : 60;
        };
    };
};

/* cache request structure */
#define CACHE_REQ_INFLIGHT 64
#ifndef MEMMAP_CACHE_REQ 
    #define MEMMAP_CACHE_REQ (sbuf)
#endif

/* cache interface */
// TODO: hold pointer?
typedef union {
    uint64_t ser;
    struct {
        uint32_t slot;
        uint32_t cache;
    };
} cache_token_t;
#define cache_token_slot(token) (token.slot)
#define cache_token_set(offset) offset
#define cache_token_ser(token) (token.ser)
#define cache_token_deser(token,wr) token.ser=(wr)

struct cache_meta {
    uint64_t tag;
    union {
        uint64_t newtag;
        struct {
            // flags
            uint8_t status : 4;
            uint8_t _unused : 4;
            uint8_t version;
            uint8_t flags;
            uint8_t group;
            // info for eviction
            int access;
        };
    };
};

enum {
    CACHE_FLAGS_ACQUIRE = 1 << 0,
    CACHE_FLAGS_DIRTY   = 1 << 1,
    CACHE_FLAGS_RACCESS = 1 << 2
};

typedef unsigned cache_t;

// init, should be called only after common init
void cache_init();

// create
cache_t cache_create(unsigned size, unsigned linesize, void * metabase, void * linebase);
cache_t cache_create_ronly(unsigned size, unsigned linesize, void * linebase);

enum {
    CACHE_ACCESS_NONE = 0,
    CACHE_ACCESS_ACQUIRE = 1,
};

// Access Level, token interface
void cache_acquire(cache_t cache, intptr_t addr, size_t size, cache_token_t *tokens, int acquire);
void cache_release(cache_token_t *tokens, int cnt);

// TODO: consider inline
// TODO: fixed base?
cache_token_t cache_request(cache_t cache, intptr_t addr);

void cache_sync(cache_token_t token);
void cache_await(cache_token_t token);
void * cache_access(cache_token_t token);
void * cache_access_mut(cache_token_t token);

#ifdef __cplusplus
}
#endif
#endif
