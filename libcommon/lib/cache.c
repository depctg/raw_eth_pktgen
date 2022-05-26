#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "common.h"
#include "cache.h"

// TODO: Constant Propogation?
#define OPT_NUM_CACHE 16

// Create and destory of cache object
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

#define token_get_cache(token,field) \
    ((caches[token.cache]).field)
#define token_get_meta(token,field) \
    cache_get_meta(token.cache,token,field)
#define token_get_line(token) \
    cache_get_line(token.cache,token)

/* request level */
static struct cache_req * req_buf;
static int head = 0, tail = 0;

void cache_init() {
    req_buf = MEMMAP_CACHE_REQ;
};

// TODO: multiple lines
// TODO: write notification by sge
static inline void cache_post(cache_token_t token, int type) {
    int ret;
    struct ibv_sge sge[2], rsge;
    struct ibv_send_wr wr, *bad_wr;
    struct ibv_recv_wr rwr, *bad_rwr;

    /* Fill the buf */
    unsigned cur = head;
    head = (head + 1) % CACHE_REQ_INFLIGHT;

    req_buf[cur].tag = token_get_meta(token,tag);
    req_buf[cur].newtag = token_get_meta(token,newtag);
    req_buf[cur].type = type;

    /* Send Packets */
    sge[0].addr = (uint64_t)(req_buf + cur);
    sge[0].length = sizeof(struct cache_req);
    sge[0].lkey = smr->lkey;

    // sge 1 for accessing cache line
    sge[1].addr = (uint64_t)(token_get_line(token));
    sge[1].length = token_get_cache(token,linesize);
    sge[1].lkey = rmr->lkey;

    wr.num_sge = type == CACHE_REQ_WRITE ? 2 : 1;
    wr.sg_list = &sge[0];
    wr.opcode = IBV_WR_SEND;
    wr.next = NULL;

    wr.send_flags = 0;
#if SEND_CMPL
    wr.send_flags |= IBV_SEND_SIGNALED;
#endif
#if SEND_INLINE
    if (token_get_cache(token,linesize) < 512)
        wr.send_flags |= IBV_SEND_INLINE;
#endif

    ret = ibv_post_send(qp, &wr, &bad_wr);
#ifndef NDEBUG
    if (unlikely(ret) != 0) {
        fprintf(stderr, "failed in post send\n");
        exit(1);
    }
#endif

    // need reply?
    if (type == CACHE_REQ_READ || type == CACHE_REQ_EVICT) {
        // mark cache as need reply
        token_get_meta(token,status) = CACHE_SYNC;

        cur++;
        if (type == CACHE_REQ_READ)
            rsge.addr = (uint64_t)(token_get_line(token));
        else // if is eviction TODO: check this
            rsge.addr = (uint64_t)(token_get_line(token));
        rsge.length = token_get_cache(token,linesize);
        rsge.lkey = rmr->lkey;

        rwr.num_sge = 1;
        rwr.sg_list = &rsge;
        rwr.wr_id = cache_token_ser(token);
        rwr.next = NULL;
    }

    ret = ibv_post_recv(qp, &rwr, &bad_rwr);
#ifndef NDEBUG
    if (unlikely(ret) != 0) {
        fprintf(stderr, "failed in post send\n");
        exit(1);
    }
#endif

}

static inline void cache_poll() {
    // poll the queue
    struct ibv_wc wc[MAX_POLL];

    int n = ibv_poll_cq(cq, MAX_POLL, wc);

    for (int i = 0; i < n; i++) {
        if (wc[i].opcode == IBV_WC_RECV && wc[i].status == IBV_WC_SUCCESS) {
            cache_token_t token;
            cache_token_deser(token,wc[i].wr_id);

            // for SGE write, no need to copy around
            token_get_meta(token,status) = CACHE_READY;
            // clear statics?
            token_get_meta(token,access) = 0;
        } else if (wc[i].opcode == IBV_WC_SEND)
            tail ++;
    }

    tail = tail % CACHE_REQ_INFLIGHT;

}

cache_t cache_create(unsigned size, unsigned linesize, void * metabase, void * linebase) {
    // TODO: more assert
    caches[cache_cnt].metabase = metabase;
    caches[cache_cnt].linebase = linebase;
    caches[cache_cnt].linesize = linesize;
    caches[cache_cnt].size = size;
    caches[cache_cnt].slots = size / linesize;
    return cache_cnt++;
}

static inline uint64_t cache_tag(cache_t cache, intptr_t addr) {
    return (uint64_t)addr & ~((uint64_t)(cache_get(cache,linesize)-1));
}

// TODO: cache_poll_token
void cache_await(cache_token_t token) {
    while (cache_get_meta(token.cache,token,status) == CACHE_SYNC)
        cache_poll();
}

// TODO: inline cachesize?
void * cache_access(cache_token_t token) {
    return caches[token.cache].linebase + token.slot * caches[token.cache].linesize;
}

inline void cache_evict(cache_token_t token, intptr_t addr) {
    token_get_meta(token, newtag) = cache_tag(token.cache, addr);
    cache_post(token, CACHE_REQ_EVICT);
}

/* main cache functions */
static inline cache_token_t _cache_select_token_groupassoc(cache_t cache, uint64_t tag) {
    cache_token_t token;
    token.cache = cache;
    unsigned linesize = cache_get(cache,linesize);
    // TODO: fix this
    token.slot = (tag / linesize) & (cache_get(cache,size)/linesize - 1);
    return token;
}

cache_token_t cache_request(cache_t cache, intptr_t addr) {
    // find slot and eviction
    uint64_t tag = cache_tag(cache, addr);
    cache_token_t token = _cache_select_token_groupassoc(cache, tag);
    if (cache_get_meta(cache, token, status) == CACHE_IDLE) {
        cache_post(token, CACHE_REQ_READ);
    } else if (cache_get_meta(cache, token, tag) != tag) {
        // TODO: process acquired lock
#ifndef NDEBUG
        if (cache_get_meta(cache, token, status) != CACHE_READY)
            eprintf(-1, "Error in cache line status");
#endif
        // do eviction
        cache_get_meta(cache, token, newtag) = tag;
        cache_post(token, CACHE_REQ_EVICT);
    }
    return token;
}

void cache_sync(cache_token_t token) {
    // TODO coherent?
    if (token_get_meta(token, status) == CACHE_READY)
        cache_post(token, CACHE_REQ_WRITE);
}

// should be called without any overlap
// without "lease" should not return empty value
void cache_acquire(cache_t cache, intptr_t addr, size_t size,
        cache_token_t *tokens, int acquire) {
    uint64_t line;
    cache_token_t *cur = tokens;

    for (line = cache_tag(cache, line);
            line < addr + size;
            line += cache_get(cache,linesize)) {
        cache_token_t token = cache_request(cache,addr);

        // TODO: finish acquire and release
        // token_set_flag(cache,token,CACHE_FLAGS_ACQUIRE,acquire);

        // collect the tokens
        if (tokens != NULL)
            *(cur++) = token;
    }
}

void cache_release(cache_token_t *tokens, int cnt) {
    // TODO: finish acquire and release
    /*
    for (int i = 0; i < cnt; i++)
        cache_set_flag(cache,tokens[i],CACHE_FLAGS_ACQUIRE,0);
    */
}


