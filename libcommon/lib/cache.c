#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "cache.h"


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

/* request level */
static struct cache_req * req_buf;
static int head = 0, tail = 0;

void cache_init() {
    req_buf = MEMMAP_CACHE_REQ;
};

// TODO: multiple lines
// TODO: write notification by sge
// TODO: inline this funciton?
static inline void cache_post(cache_token_t token, int type) {
    struct ibv_sge sge[2], rsge;
    struct ibv_send_wr wr, *bad_wr;
    struct ibv_recv_wr rwr, *bad_rwr;

    dprintf("cache %d, token offset %u, tag %p, newtag %p, op %d", token.cache, 
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

/* cache policy funcitons */

/* Group assoc */

static inline unsigned rand_next() {
    const int mult = 22695477;
    const int inc = 1;
    static unsigned cur = 2333;
    return (cur = cur * mult + inc);
}
const int group_bits = 2;
const int groups = 1 << group_bits;

static void inline _cache_access_markdirty(cache_token_t token, int mut) {
    // mark line as dirty
    if (mut)
        token_set_flag(token, CACHE_FLAGS_DIRTY);
}

static void inline _cache_access_groupassoc(cache_token_t token, int mut) {
    // mark line as dirty
    _cache_access_markdirty(token, mut);
    // mark self as assoc
    unsigned base = token.slot & ~(groups-1);
    for (int i = 0; i < groups; i++) {
        cache_token_t tk = (cache_token_t){.cache=token.cache, .slot=base+i};
        token_reset_flag(tk,CACHE_FLAGS_RACCESS);
    }
    token_set_flag(token,CACHE_FLAGS_RACCESS);
}

/* direct assoc */
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
    return t[rand_next() % groups];
}

static inline cache_token_t _cache_select_groupassoc_lru(cache_t cache, uint64_t tag) {

    // check if direct is the target tag
    cache_token_t direct_token = _cache_select_directassoc(cache, tag);
    if (cache_get_meta(cache, direct_token, tag) == tag) return direct_token;

    // try group assoc
    unsigned linesize = cache_get(cache,linesize);
    unsigned base = (tag/linesize/groups) & (cache_get(cache,size)/linesize/groups - 1);
    base <<= group_bits;
    cache_token_t t, tlru;
    for (int i = 0; i < groups; i++) {
        t.cache=cache;
        t.slot=base+i;
        if (token_get_meta(t,status) == CACHE_IDLE)
            return t;
        if (!token_check_flag(t,CACHE_FLAGS_RACCESS)) // ?
            return t;
        tlru = t;
    }
    // random select a single element to evict
    return tlru;
}


// TODO: change this to apply to different functions
#define __cache_access_handler _cache_access_groupassoc
#define __cache_select _cache_select_directassoc

cache_token_t cache_request(cache_t cache, intptr_t addr) {
    // find slot and eviction
    uint64_t tag = cache_tag(cache, addr);
    cache_token_t token = __cache_select(cache, tag);
    dprintf("translate addr %lx tag %lx cache %d, slot %d, slotag %lx",
            addr, tag, token.cache, token.slot, token_get_meta(token,tag));
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

void * cache_access(cache_token_t token) {
    __cache_access_handler(token, 0);
    return token_get_line(token);
}

void * cache_access_mut(cache_token_t token) {
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

        // TODO: finish acquire and release
        // token_set_flag(cache,token,CACHE_FLAGS_ACQUIRE,acquire);

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
static inline void cache_evict(cache_token_t token, intptr_t addr) {
    uint64_t tag = cache_tag(token.cache, addr);
    token_get_meta(token, newtag) = token_get_meta(token, tag);
    token_get_meta(token, tag) = tag;
    cache_post(token, CACHE_REQ_EVICT);
}


