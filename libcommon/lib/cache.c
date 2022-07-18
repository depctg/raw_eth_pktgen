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
#include "helper.h"

/* request level */
static struct cache_req * req_buf;
static int head = 0, nout = 0;

void cache_init() {
    req_buf = MEMMAP_CACHE_REQ;
};

struct HeapNode {
    double dist;
    int v;
};

// update tail at the end of poll
// might have problem if multi-threaded
static inline void cache_poll() {
    // poll the queue
    struct ibv_wc wc[MAX_POLL];

    int n = ibv_poll_cq(cq, MAX_POLL, wc);
    if (n > 0) dprintf("%d", n);
    for (int i = 0; i < n; i++) {
        if (wc[i].status != IBV_WC_SUCCESS) {
            printf("WR failed\n");
            exit(1);
        }
        if (wc[i].opcode == IBV_WC_RECV) {
            line_header *h = (line_header *) wc[i].wr_id;
            h->status = LINE_READY;
            // clear statics?
            h->weight = 0;
            h->flags = 0;
        } else if (wc[i].opcode == IBV_WC_SEND) {
            nout --;
        }
    }
}

// TODO: multiple lines
// TODO: write notification by sge
// TODO: inline this funciton?
// Done: check head/tail
static inline void cache_post(const cache_token_t *token, int type, uint64_t tag2) {
    struct ibv_sge sge[2], rsge;
    struct ibv_send_wr wr, *bad_wr;
    struct ibv_recv_wr rwr, *bad_rwr;

    cache_t cache = token->cache;
    uint64_t tag = token->tag;
    unsigned slot = token_header_field(token,slot);
    
    dprintf("cache %d, token slot %u, tag %lu, tag2 %lu, op %d", cache, slot, tag, tag2, type);

    /* Fill the buf */
    while (nout >= CACHE_REQ_INFLIGHT) {
        cache_poll();
    }
    unsigned cur = head;
    head = (head + 1) % CACHE_REQ_INFLIGHT;
    nout ++;

    req_buf[cur].tag = tag;
    req_buf[cur].tag2 = tag2;
    req_buf[cur].type = type;
    req_buf[cur].cache_id = cache;

    /* Send Packets */
    sge[0].addr = (uint64_t)(req_buf + cur);
    sge[0].length = sizeof(struct cache_req);
    sge[0].lkey = smr->lkey;

    // sge 1 for accessing cache line
    sge[1].addr = (uint64_t)(cache_get_ldata(cache,slot));
    sge[1].length = cache_get_field(cache,linesize);
    sge[1].lkey = rmr->lkey;

    wr.num_sge = type == CACHE_REQ_READ ? 1 : 2;
    wr.sg_list = &sge[0];
    wr.opcode = IBV_WR_SEND;
    wr.wr_id = tag;
    wr.next = NULL;

    wr.send_flags = 0;
#if SEND_CMPL
    wr.send_flags |= IBV_SEND_SIGNALED;
#endif
#if SEND_INLINE
    if (cache_get_field(cache,linesize) < 512)
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
        rsge.addr = (uint64_t)(cache_get_ldata(cache,slot));
        rsge.length = cache_get_field(cache,linesize);
        rsge.lkey = rmr->lkey;

        rwr.num_sge = 1;
        rwr.sg_list = &rsge;
        rwr.wr_id = (uint64_t) token->head_addr; //line header as ser
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


cache_t cache_create(unsigned size, unsigned linesize, void * linebase) {
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

    // initialize header
    for (unsigned i = 0; i < caches[cache_cnt].slots; ++ i) {
        line_header *h = (line_header *) (caches[cache_cnt].linebase + i * (sizeof(line_header) + linesize));
        h->slot = i;
    }
    return cache_cnt++;
}

// TODO: cache_poll_token
void cache_await(cache_token_t *token) {
    dprintf("Await token slot %u, status %d", token_header_field(token,slot), (int)token_header_field(token,status));
    // while (cache_header_field(token->cache,token->slot,status) == LINE_SYNC)
    while (token_header_field(token,status) == LINE_SYNC)
        cache_poll();
}

void cache_request(cache_t cache, intptr_t addr, cache_token_t *token) {
    // find slot and eviction
    uint64_t tag = cache_ofst_mask(cache_get_field(cache,linesize), addr);
    uint16_t ofst = cache_tag_mask(cache_get_field(cache,linesize), addr);
    token->cache = cache;
    token->tag = tag;
    token->line_ofst = ofst;

    // set line position
    __cache_select(cache, tag, token);
    dprintf("translate addr %lu tag %lu cache %d, slot %d",
            addr, tag, cache, token_header_field(token,slot));
    if (token_header_field(token,tag) == tag &&
            token_header_field(token,status) != LINE_IDLE) {
        // do nothing, just return
        dprintf("-> READY");
    } else if (token_header_field(token,status) != LINE_IDLE &&
            token_header_field(token,tag) != tag &&
            token_check_flag(token,LINE_FLAGS_DIRTY)) {

        // wait prev request to finish?
        cache_await(token);
        // do eviction
        uint64_t tag2 = token_header_field(token,tag);
        token_header_field(token,tag) = tag;
        token_header_field(token,status) = LINE_SYNC;
        dprintf("-> EVICT %lu, FETCH %lu", tag2, token_header_field(token, tag));
        cache_post(token, CACHE_REQ_EVICT, tag2);
    } else {
        token_header_field(token,tag) = tag;
        token_header_field(token,status) = LINE_SYNC;
        dprintf("-> FETCH %lu", token->tag);
        cache_post(token, CACHE_REQ_READ, -1);
    }
}

// TODO: inline ?
void cache_access_check(cache_token_t *token) {
    if (token->tag != token_header_field(token,tag))
        cache_request(token->cache, token->tag+token->line_ofst, token);
}

void * cache_access(cache_token_t *token) {
#ifdef CACHE_CONFIG_RUNTIME_CHECK
    cache_access_check(token);
#endif
    cache_await(token);
    __cache_access_handler(token, 0);
    return token_get_data(token);
}

void * cache_access_mut(cache_token_t *token) {
#ifdef CACHE_CONFIG_RUNTIME_CHECK
    cache_access_check(token);
#endif
    cache_await(token);
    __cache_access_handler(token, 1);
    return token_get_data(token);
}

void cache_sync(cache_token_t *token) {
    // TODO coherent?
    if (token_header_field(token,status) == LINE_READY
            && token_check_flag(token,LINE_FLAGS_DIRTY)) {
        cache_post(token, CACHE_REQ_WRITE, -1);
        // no need to wait
        token_clear_flag(token,LINE_FLAGS_DIRTY);
    }
}

/* calloc like token_acquire */
// overlapped token increase acq_count
// only evict acq_count = 0
// without "lease" should not return empty value
// return: list of tokens
void cache_acquire(cache_t cache, intptr_t addr, size_t nitems, size_t size, cache_token_t *tokens) {
    for (unsigned i = 0; i < nitems; ++ i) {
        cache_request(cache, addr + size*i, tokens + i);
#ifdef CACHE_CONFIG_ACQUIRE
        token_header_field(tokens+i,acq_count) ++;
#endif
    }
}

// TODO check negative ?
void cache_release(cache_token_t *tokens, int cnt) {
    for (int i = 0; i < cnt; i++)
        token_header_field(tokens+i,acq_count)--;
}

// TODO: inline cachesize?
// TODO: check token valid?
void cache_evict(cache_token_t *token, intptr_t addr) {
    uint64_t tag = cache_ofst_mask(cache_get_field(token->cache,linesize), addr);
    uint64_t tag2 = token->tag;
    token->tag = tag;
    token_header_field(token, tag) = tag;
    cache_post(token, CACHE_REQ_EVICT, tag2);
}
