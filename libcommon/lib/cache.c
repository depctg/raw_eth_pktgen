#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "common.h"
#include "cache.h"

// TODO: fixed tokens

// Create and destory of cache object
static struct cache_internal {
    // pointers
    struct cache_meta * metabase;
    char * linebase;
    // linesizes
    unsigned linesize;
    unsigned size;
} caches[OPT_NUM_CACHE];
static int cache_cnt = 0;

enum cache_status {
    // IDLE only happens on init
    CACHE_IDLE=0,
    CACHE_ALLOC,
    CACHE_READY,
    CACHE_SYNC,
    CACHE_END
}

static uint64_t inline cache_tag(cache_t cache, intptr_t addr) {
    return (uint64_t)addr & ~((uint64_t)(cache_get(cache,linesize)-1))
}

// impl macros

#define cache_get(cache,field) \
    ((cache).field)
#define cache_get_meta(cache,token,field) \
    ((cache).metabase[cache_token_offset(token)].field)
#define cache_get_line(cache,token) \
    ((cache).linebase+cache_token_offset(token)*cache.linesize)

cache_t cache_create(unsigned size, unsigned linesize, void * metabase, void * linebase) {
    // TODO: more assert
    caches[cache_cnt].metabase = metabase;
    caches[cache_cnt].linebase = linebase;
    caches[cache_cnt].linesize = linesize;
    caches[cache_cnt].size = size;
    caches[cache_cnt].slots = size / linesize;
    return cache_cnt++;
}

// eviction
static inline void eviction_notify(int name, int action) {
    event[event_cnt++] = action;
    if (unlikely(event_cnt >= EVICTION_EVENT_COMPRESS)) {
    }
}

// TODO: cache_poll_token
inline void cache_await(cache_token_t token) {
    while (cache_get_meta(cache,token,status) == CACHE_SYNC)
        cache_poll();
}

// TODO: inline cachesize?
inline void * cache_access(cache_token_t token) {
    return caches[token.cache].linebase + token.slot * caches[token.cache].linesize;
}

inline void cache_evict(cache_token_t token, intptr_t addr) {
    cache_get_meta(cache, token, newtag) = (uint64_t)cache_tag(addr);
    cache_post(buf, token, CACHE_REQ_EVICT);
}

static inline cache_token_t cache_select_token(cache_t cache, uint64_t tag) {
    cache_token_t token;
    token.cache = cache;
    // TODO: fix this
    token.slot = tag >> 4 & 0xFF;
}

/* main cache functions */
static inline cache_token_t _cache_lookup_groupassoc(cache_t cache, intptr_t addr) {
    // find slot and eviction
    uint64_t tag = cache_tag(addr);
    cache_token_t token = cache_select_token(cache, tag);
    if (cache_get_meta(cache, token, status) == CACHE_EMPTY) {
        cache_post(buf, token, CACHE_REQ_READ);
    } else if (cache_get_meta(cache, token, tag) != tag) {
        // await all inflight requests
        // TODO: process with lock
        cache_await(token);
#ifndef NDEBUG
        if (cache_get_meta(cache, token, status) != CACHE_READY)
            eprintf(-1, "Error in cache line status");
#endif
        // do eviction
        cache_get_meta(cache, token, newtag) = tag;
        cache_post(buf, token, CACHE_REQ_EVICT);
    }
    return token;
}

cache_token_t cache_request(cache_t cache, intptr_t addr) {
    return _cache_lookup_groupassoc(cache, addr);
}

// should be called without any overlap
// without "lease" should not return empty value
void cache_acquire(intptr_t addr, size_t size,
        cache_token_t *tokens, int acquire) {
    uint64_t line;
    cache_token_t *cur = tokens;

    for (line = cache_tag(line);
            line < addr + size;
            line += CACHE_LINE_SIZE) {
        cache_token_t token = cache_lookup(addr);
        token_set_flag(cache,token,CACHE_FLAGS_ACQUIRE,acquire);

        // collect the tokens
        if (tokens != NULL)
            *(cur++) = token;
    }
}

void cache_release(cache_token_t *tokens, int cnt) {
    for (int i = 0; i < cnt; i++)
        cache_set_flag(cache,tokens[i],CACHE_FLAGS_ACQUIRE,0);
}

/* request level */

struct cache_req_buf req_buf = {
    .reqs = MEMMAP_CACHE_REQ,
    .head = 0,
    .tail = 0
};

// TODO: multiple lines
// TODO: write notification by sge
static inline void cache_post(cache_token_t token, int type) {
    int ret;
    struct ibv_sge sge[2];
    struct ibv_send_wr wr[2], *bad_wr;

    /* Fill the buf */
    unsigned cur = head;
    head = (head + 1) % CACHE_REQ_INFLIGHT;

    req_buf.reqs[cur].tag = token_get_meta(token,addr);
    req_buf.reqs[cur].newtag = token_get_meta(token,newtag);
    req_buf.reqs[cur].type = type;

    /* Send Packets */
    sge[0].addr = (uint64_t)(req_buf.reqs + cur);
    sge[0].length = CACHE_REQ_SIZE;
    sge[0].lkey = smr->lkey;

    // sge 1 for accessing cache line
    sge[1].addr = (uint64_t)(token_get_line(token));
    sge[1].length = CACHE_LINE_SIZE;
    sge[1].lkey = rmr->lkey;

    wr.num_sge = type == CACHE_REQ_WRITE ? 2 : 1;
    wr.sg_list = &sge[0];
    wr.opcode = IBV_WR_SEND;
    wr.next = NULL;

    // need reply?
    if (type == CACHE_REQ_READ || type == CACHE_REQ_EVICT) {
        // mark cache as need reply
        cache_get_meta(cache,token,status) = CACHE_SYNC;

        // get next frame
        cur++;
        req_buf.reqs[cur].tag = cache_get_meta(cache,token,addr);
        req_buf.reqs[cur].newtag = cache_get_meta(cache,token,newtag);
        req_buf.reqs[cur].type = type;

        wr[1].num_sge = 1;
        wr[1].sg_list = &sge[1];
        wr[1].opcode = IBV_WR_RECV;
        wr[1].wr_id = cache_token_wr(token);
        wr[1].next = NULL;

        wr[0].next = &wr[1];
    }


    wr.send_flags = 0;
#if SEND_CMPL
    wr.send_flags |= IBV_SEND_SIGNALED;
#endif
#if SEND_INLINE
    if (likely(size < 512))
        wr.send_flags |= IBV_SEND_INLINE;
#endif

    ret = ibv_post_send(qp, wr, &bad_wr);
#ifndef NDEBUG
    if (unlikely(ret) != 0) {
        fprintf(stderr, "failed in post send\n");
        exit(1);
    }
#endif
}

static inline void cache_poll(int n) {
    // poll the queue
    struct ibv_wc wc[MAX_POLL];

    int n = ibv_poll_cq(cq, MAX_POLL, wc);
    tail = (tail + n) % CACHE_REQ_INFLIGHT;

    for (int i = 0; i < n; i++) {
        if (wc[i].opcode == IBV_WC_RECV && wc[i].status == IBV_WC_SUCCESS) {
            unsigned token;
            cache_token_deser(token,wc[i].wr_id);

            // for SGE write, no need to copy around
            cache_get_meta(cache,token,status) = CACHE_READY;
            // clear statics?
            cache_get_meta(cache,token,access) = 0;
        }
    }
}

