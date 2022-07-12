#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "cache.h"
#include "cache_internal.h"
#include "cache_policy.h"
#include "common.h"

/* request level */
static struct cache_req * req_buf;
static int head = 0, tail = 0;

void cache_init() {
    req_buf = MEMMAP_CACHE_REQ;
};

// TODO: multiple lines
// TODO: write notification by sge
// TODO: inline this funciton?
// TODO: check head/tail
static inline void cache_post(cache_token_t token, int type) {
    struct ibv_sge sge[2], rsge;
    struct ibv_send_wr wr, *bad_wr;
    struct ibv_recv_wr rwr, *bad_rwr;

    dprintf("cache %d, token offset %u, tag %lu, newtag %lu, op %d", token.cache, 
            token.slot, token_get_meta(token,tag), token_get_meta(token,newtag), type);

    /* Fill the buf */
    unsigned cur = head;
    head = (head + 1) % CACHE_REQ_INFLIGHT;

    req_buf[cur].tag = token_get_meta(token,tag);
    req_buf[cur].newtag = token_get_meta(token,newtag);
    req_buf[cur].type = type;
    req_buf[cur].cache_id = token.cache;

    /* Send Packets */
    sge[0].addr = (uint64_t)(req_buf + cur);
    sge[0].length = sizeof(struct cache_req);
    sge[0].lkey = smr->lkey;

    // sge 1 for accessing cache line
    sge[1].addr = (uint64_t)(token_get_line(token));
    sge[1].length = token_get_cache(token,linesize);
    sge[1].lkey = rmr->lkey;

    wr.num_sge = type == CACHE_REQ_READ ? 1 : 2;
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

    int ret = ibv_post_send(qp, &wr, &bad_wr);
#ifndef NDEBUG
    if (unlikely(ret) != 0) {
        fprintf(stderr, "failed in post send\n");
        exit(1);
    }
#endif

    // need reply?
    if (type == CACHE_REQ_READ || type == CACHE_REQ_EVICT) {
        rsge.addr = (uint64_t) (token_get_line(token));
        rsge.length = token_get_cache(token,linesize);
        rsge.lkey = rmr->lkey;

        rwr.num_sge = 1;
        rwr.sg_list = &rsge;
        rwr.wr_id = cache_token_ser(token);
        rwr.next = NULL;

        ret = ibv_post_recv(qp, &rwr, &bad_rwr);
#ifndef NDEBUG
        if (unlikely(ret) != 0) {
            fprintf(stderr, "failed in post send\n");
            exit(1);
        }
#endif
    }

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
            dprintf("poll cache %d slot %d", token.cache, token.slot);
            token_get_meta(token,status) = CACHE_READY;
            // clear statics?
            token_get_meta(token,access) = 0;
            token_get_meta(token,flags) = 0;
        } else if (wc[i].opcode == IBV_WC_SEND)
            tail ++;
    }

    tail = tail % CACHE_REQ_INFLIGHT;
}

cache_t cache_create(unsigned size, unsigned linesize, void * metabase, void * linebase) {
    // assert(is_pow2(size));
    assert(is_pow2(linesize));
    // assert(linesize > 16); // required by tag layout
    assert(linesize <= CACHE_LINE_LIMIT);
    assert(size >= linesize);

    // TODO: more assert
    caches[cache_cnt].metabase = metabase;
    caches[cache_cnt].linebase = linebase;
    caches[cache_cnt].linesize = linesize;
    caches[cache_cnt].size = size;
    caches[cache_cnt].slots = size / linesize;
    return cache_cnt++;
}

cache_t cache_create_ronly(unsigned size, unsigned linesize, void * linebase) {
    // assert(is_pow2(size));
    assert(is_pow2(linesize));
    // assert(linesize > 16); // required by tag layout
    assert(linesize <= CACHE_LINE_LIMIT);
    assert(size >= linesize);

    // TODO: more assert
    caches[cache_cnt].linebase = linebase;
    caches[cache_cnt].linesize = linesize;
    caches[cache_cnt].size = size;
    caches[cache_cnt].slots = size / linesize;
    caches[cache_cnt].metabase = calloc(caches[cache_cnt].slots, sizeof(struct cache_meta));
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


cache_token_t cache_request(cache_t cache, intptr_t addr) {
    // find slot and eviction
    uint64_t tag = cache_tag(cache, addr);
    cache_token_t token = __cache_select(cache, tag);
    dprintf("translate addr %lx tag %lx cache %d, slot %d, slotag %lx",
            addr, tag, token.cache, token.slot, token_get_meta(token,tag));

#if CACHE_CONFIG_RUNTIME_CHECK
    cache_get_meta(cache, token, version)++;
    token.ver++;
#endif // CACHE_CONFIG_RUNTIME_CHECK

    if (cache_get_meta(cache, token, tag) == tag &&
            cache_get_meta(cache, token, status) != CACHE_IDLE) {
        // do nothing, just return
        dprintf("-> READY");
    } else if (cache_get_meta(cache, token, status) != CACHE_IDLE &&
            cache_get_meta(cache, token, tag) != tag &&
            cache_check_flag(cache, token, CACHE_FLAGS_DIRTY)) {
        // wait prev request to finish?
        while (cache_get_meta(token.cache,token,status) == CACHE_SYNC)
            cache_poll();
        // do eviction
        cache_get_meta(cache, token, newtag) = cache_get_meta(cache, token, tag);
        cache_get_meta(cache, token, tag) = tag;
        cache_get_meta(cache, token, status) = CACHE_SYNC;
        dprintf("-> EVICT %lx, FETCH %lx", token_get_meta(token, newtag), token_get_meta(token, tag));
        cache_post(token, CACHE_REQ_EVICT);
    } else {
        cache_get_meta(cache, token, tag) = tag;
        cache_get_meta(cache, token, status) = CACHE_SYNC;
        dprintf("-> FETCH: s %d, tag %lx, flag %x", token_get_meta(token, status), token_get_meta(token, tag), token_get_meta(token,flags));
        cache_post(token, CACHE_REQ_READ);
    }
    return token;
}

void cache_access_check(cache_token_t *token, intptr_t addr) {
    if (token.ver != token_get_meta(token, version))
        *token = cache_request(token.cache, addr);
}

void * cache_access(cache_token_t &token) {
    __cache_access_handler(token, 0);
    return token_get_line(token);
}

void * cache_access_mut(cache_token_t &token) {
    __cache_access_handler(token, 1);
    return token_get_line(token);
}


void cache_sync(cache_token_t token) {
    // TODO coherent?
    if (token_get_meta(token, status) == CACHE_READY
            && token_check_flag(token, CACHE_FLAGS_DIRTY)) {
        cache_post(token, CACHE_REQ_WRITE);
        token_reset_flag(token, CACHE_FLAGS_DIRTY);
    }
}

// should be called without any overlap
// without "lease" should not return empty value
void cache_acquire(cache_t cache, intptr_t addr, size_t size,
        cache_token_t *tokens, int acquire) {
    uint64_t line;
    cache_token_t *cur = tokens;

    for (line = cache_tag(cache, addr);
            line < addr + size;
            line += cache_get(cache,linesize)) {
        cache_token_t token = cache_request(cache,addr);

#if CACHE_CONFIG_ACQUIRE = 1
        token_set_flag(token, CACHE_FLAGS_ACQUIRE, acquire);
#endif

        // collect the tokens
        if (tokens != NULL)
            *(cur++) = token;
    }
}

void cache_release(cache_token_t *tokens, int cnt) {
    for (int i = 0; i < cnt; i++)
        token_reset_flag(tokens[i],CACHE_FLAGS_ACQUIRE);
}


// TODO: inline cachesize?
void cache_evict(cache_token_t token, intptr_t addr) {
    uint64_t tag = cache_tag(token.cache, addr);
    token_get_meta(token, newtag) = token_get_meta(token, tag);
    token_get_meta(token, tag) = tag;
    cache_post(token, CACHE_REQ_EVICT);
}


