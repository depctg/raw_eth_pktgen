#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <assert.h>
#include <errno.h>

#include "common.h"
#include "helper.h"
#include "cache.h"
#include "cache_policy.h"
#include "cache_internal.h"
#include "stats.h"
#include "stats_internal.h"
#include "stack_like.h"
#include "local_mr.h"

/* request level */
static struct cache_req * req_buf;
// head, nout not used
// post work will block if is full
static int head = 0;
static int nout = 0;

void cache_init() {
  req_buf = MEMMAP_CACHE_REQ;

  // init space for local memory interface
  stack_init();
  local_mr_init();
}

static inline void _cache_poll() {
    static struct ibv_wc wc[MAX_POLL];

    int n = ibv_poll_cq(cq, MAX_POLL, wc);
    for (int i = 0; i < n; ++ i) {
        if (wc[i].status != IBV_WC_SUCCESS) {
            printf("WR failed\n");
            exit(1);
        }
        if (wc[i].opcode == IBV_WC_RECV) {
            line_header *h = (line_header *) wc[i].wr_id;
            dprintf("poll cq tag %lu", h->tag);
            // for SGE write, no need to copy around
            h->status = LINE_READY;
            // clear statics?
        } else if (wc[i].opcode == IBV_WC_SEND) {
            nout --;
            // fdprintf("sout now %d", now);
        } 
    }
}

// TODO: multiple lines
// TODO: write notification by sge
// TODO: inline this funciton?
static inline void cache_post(cache_token_t token, int type, uint64_t tag2) {
    /* prepare work packet */
    struct ibv_sge s_sge[2], r_sge;
    struct ibv_send_wr swr, *bad_wr;
    struct ibv_recv_wr rwr, *bad_rwr;

    /* populate work packet */
    cache_t cache = token.cache;
    uint64_t tag = token.tag;
    unsigned slot = token.slot;

    dprintf("cache %d, token slot %u, tag %lu, tag2 %lu, op %d", cache, slot, tag, tag2, type);
    while (nout >= CACHE_REQ_INFLIGHT) {
        _cache_poll();
    }

    /* Fill the buf */
    unsigned cur = head;
    head = (head + 1) & (CACHE_REQ_INFLIGHT - 1);
    nout ++;

    req_buf[cur].tag = tag;
    req_buf[cur].tag2 = tag2;
    req_buf[cur].type = type;
    req_buf[cur].cache_id = cache;


    /* Send Packets */
    s_sge[0].addr = (uint64_t)(req_buf + cur);
    s_sge[0].length = sizeof(struct cache_req);
    s_sge[0].lkey = smr->lkey;

    // sge 1 for accessing cache line
    s_sge[1].addr = (uint64_t)(cache_get_line(cache,slot));
    s_sge[1].length = cache_get_field(cache,linesize);
    s_sge[1].lkey = rmr->lkey;

    swr.num_sge = type == CACHE_REQ_READ ? 1 : 2;
    swr.sg_list = &s_sge[0];
    swr.opcode = IBV_WR_SEND;
    // wr_id is the address of cache line header 
    // where the received data is expected to be placed at
    // only useful when the request is expecting a reply
    swr.wr_id = 0;
    swr.next = NULL;

    swr.send_flags = 0;
#if SEND_CMPL
    swr.send_flags |= IBV_SEND_SIGNALED;
#endif
#if SEND_INLINE
    if (cache_get_field(cache,linesize) < 512)
        swr.send_flags |= IBV_SEND_INLINE;
#endif
    int ret = ibv_post_send(qp, &swr, &bad_wr);

#ifndef NDEBUG
    if (unlikely(ret) != 0) {
        fprintf(stderr, "failed in post send\n");
        exit(1);
    }
#endif

    // need reply?
    if (type == CACHE_REQ_READ || type == CACHE_REQ_EVICT) {
        r_sge.addr = (uint64_t) (cache_get_line(cache,slot));
        r_sge.length = cache_get_field(cache,linesize);
        r_sge.lkey = rmr->lkey;

        rwr.num_sge = 1;
        rwr.wr_id = token_header_ptr2int(token);
        rwr.sg_list = &r_sge;
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


cache_t cache_create(unsigned size, unsigned linesize) {
    // user should not manage starting address of 
    // the cache line base
    static char *_start_base = NULL;
    if (!_start_base)
        _start_base = rbuf;

    assert(is_pow2(size));
    assert(is_pow2(linesize));
    assert(linesize <= CACHE_LINE_LIMIT);
    // TODO: conditional
    assert(size / linesize >= groups);

    // TODO: more assert
    caches[cache_cnt].linebase = _start_base;
    caches[cache_cnt].linesize = linesize;
    caches[cache_cnt].size = size;
    caches[cache_cnt].slots = size / linesize;
    caches[cache_cnt].metabase = aligned_alloc(16, cache_get_field(cache_cnt, slots) * sizeof(struct line_header));

	// initialize header
	for (unsigned i = 0; i < caches[cache_cnt].slots; ++ i) {
        cache_header_field(cache_cnt, i, slot) = i;
        cache_header_field(cache_cnt, i, status) = LINE_IDLE;
        cache_header_field(cache_cnt, i, acq_count) = 0;
    }

    _start_base += size;
    return cache_cnt++;
}

static inline void cache_await(cache_token_t token) {
    dprintf("Await token slot %u, status %d", token_header_field(token,slot), token_header_field(token,status));
	if (likely(token_header_field(token,status) != LINE_SYNC)) {
		return;
	}

    // clear inque work complete
    _cache_poll();
	if (likely(token_header_field(token,status) != LINE_SYNC)) {
		return;
	}
    ldf_inc(token.cache);

	do {
        _cache_poll();
	} while (token_header_field(token,status) == LINE_SYNC);
}

cache_token_t cache_request(uint64_t vaddr) {
    virt_addr_t ser = {.ser = vaddr};
    uint64_t addr = ser.addr;
    cache_t cache = ser.cache;
    dprintf("requesting from %d addr: %lu", cache, addr);

    if (unlikely(cache < CACHE_ID_OFFSET )) {
        // requesing local memory
        // use tag to store access address
        cache_token_t tk;
        tk.cache = cache;
        tk.tag = addr;
        return tk;
    }

    // log number of reqs
    req_inc(cache);
    uint64_t tag = cache_ofst_mask(cache_get_field(cache,linesize), addr);
    uint16_t ofst = cache_tag_mask(cache_get_field(cache,linesize), addr);

    // find slot and eviction
    cache_token_t token = __cache_select(cache, tag);
    token.cache = cache;
    token.tag = tag;
    token.line_ofst = ofst;

    // log req event
    add_trace(token, EVNT_REQ);

    dprintf("translate addr %lu tag %lu cache %d, slot %u old tag %lu", addr, tag, cache, token.slot, token_header_field(token, tag));
    if (token_header_field(token, status) == LINE_IDLE) {
        token_header_field(token,tag) = tag;
        token_header_field(token,status) = LINE_READY;
        miss_inc(cache, tag/cache_get_field(cache,linesize));
    }
    if (token_header_field(token,tag) == tag) {
        dprintf("-> tag match, do nothing");
    } else {
        miss_inc(cache, tag/cache_get_field(cache,linesize));
        // if prefetch flag is still on, count as inaccurate pref
#ifdef CACHE_LOG_PREF
        if (token_check_flag(token,LINE_FLAGS_PREFED)) {
            inacc_pref_inc(cache);
            token_clear_flag(token,LINE_FLAGS_PREFED);
        } 
#endif
        // wait prev req ?
        if (token_header_field(token,status) != LINE_IDLE) cache_await(token);

        if (token_check_flag(token,LINE_FLAGS_DIRTY)) {
            // eviction
            uint64_t tag2 = token_header_field(token,tag);
            token_header_field(token,tag) = tag;
            token_header_field(token,status) = LINE_SYNC;
            token_header_field(token,weight) = 0;
            token_header_field(token,flags) = 0;
            dprintf("-> EVICT %lu, FETCH %lu", tag2, token_header_field(token,tag));
            // printf("-> EVICT %lu, FETCH %lu\n", tag2, token_header_field(token,tag));
            cache_post(token, CACHE_REQ_EVICT, tag2);
        } else {
            // fetch
            token_header_field(token,tag) = tag;
            // printf("-> FETCH: %lu\n", token_header_field(token,tag));
            token_header_field(token,status) = LINE_SYNC;
            token_header_field(token,weight) = 0;
            token_header_field(token,flags) = 0;
            dprintf("-> FETCH: %lu", token_header_field(token,tag));
            cache_post(token, CACHE_REQ_READ, -1);
        }
    }
    return token;
}

void cache_prefetch(intptr_t vaddr) {
#ifdef CACHE_LOG_PREF
    virt_addr_t ser = {.ser = vaddr};
    uint64_t addr = ser.addr;
    cache_t cache = ser.cache;
    pref_inc(cache);
#endif
    cache_request(vaddr);
#ifdef CACHE_LOG_PREF
    token_set_flag(token,LINE_FLAGS_PREFED);
#endif
}

// 0, 1 are local
// return dat directly
static inline void * placement_check(cache_token_t token) {
    if (token.cache == 0) return stack_access(token.tag);
    if (token.cache == 1) return local_mr_access(token.tag);
    return NULL;
}

// TODO: inline ?
void cache_access_check(cache_token_t *token) {
    if (token->tag != token_header_field(*token,tag)) {
        virt_addr_t ser = { .cache = token->cache, .addr = (token->tag + token->line_ofst)};
        *token = cache_request(ser.ser);
    }
}

void * cache_access(cache_token_t *token) {
    void *dat = placement_check(*token);
    if (dat) return dat;
    // add addr trace
    access_inc(token->cache);

    cache_access_check(token);
    cache_await(*token);
    __cache_access_handler(*token, 0);
#ifdef CACHE_LOG_PREF
    token_clear_flag(token,LINE_FLAGS_PREFED);
#endif
    add_trace(*token, EVNT_ACC);
    return token_get_data(*token);
}

void * cache_access_mut(cache_token_t *token) {
    if (token->cache == 0) {
        return stack_access(token->tag);
    }
    access_inc(token->cache);
    cache_access_check(token);
    cache_await(*token);
    __cache_access_handler(*token, 1);
#ifdef CACHE_LOG_PREF
    token_clear_flag(token,LINE_FLAGS_PREFED);
#endif
    add_trace(*token, EVNT_ACC_MUT);
    return token_get_data(*token);
}

void * cache_access_nrtc(cache_token_t *token) {
    if (token->cache == 0) {
        return stack_access(token->tag);
    }
    access_inc(token->cache);
    cache_await(*token);
    __cache_access_handler(*token, 0);
#ifdef CACHE_LOG_PREF
    token_clear_flag(token,LINE_FLAGS_PREFED);
#endif
    add_trace(*token, EVNT_ACC);
    return token_get_data(*token);
}

void * cache_access_nrtc_mut(cache_token_t *token) {
    if (token->cache == 0) {
        return stack_access(token->tag);
    }
    access_inc(token->cache);
    cache_await(*token);
    __cache_access_handler(*token, 1);
#ifdef CACHE_LOG_PREF
    token_clear_flag(token,LINE_FLAGS_PREFED);
#endif
    add_trace(*token, EVNT_ACC_MUT);
    return token_get_data(*token);
}

// void cache_sync(cache_token_t *token) {
//     // TODO coherent?
//     if (token_get_status(token) == LINE_READY 
//             && token_check_flag(token,LINE_FLAGS_DIRTY)) {
//         cache_post(token, CACHE_REQ_WRITE, -1);
//         token_clear_flag(token,LINE_FLAGS_DIRTY);
//     }
// }

/* calloc like token_acquire */
// overlapped address on the same cache line increase acq_count
// return: list of tokens
void cache_acquire(intptr_t vaddr, size_t nitems, size_t size, cache_token_t *tokens) {
    for (unsigned i = 0; i < nitems; ++ i) {
        tokens[i] = cache_request(vaddr + size*i);
#ifdef CACHE_CONFIG_ACQUIRE
        token_header_field(tokens[i],acq_count) ++;
#endif
        add_trace(tokens[i], EVNT_ACQ);
    }
}

void cache_re_acquire(cache_token_t *token) {
    if (unlikely(token->cache == 0)) {
        // access stack mem
        return;
    }
    // fdprintf("reacq addr %lu tag %lu cache %d, slot tag %lu", token->tag+token->line_ofst, token->tag, token->cache, token_header_field(token,tag)); 
    cache_access_check(token);
#ifdef CACHE_CONFIG_ACQUIRE
    token_header_field(*token,acq_count) ++;
#endif
    add_trace(*token, EVNT_ACQ);
}

// check negative, prevent aliasing
void cache_release(cache_token_t *tokens, int cnt) {
    for (int i = 0; i < cnt; i++) {
        if (tokens[i].cache == 0) continue;
        if (token_header_field(tokens[i],acq_count) > 0) {
            token_header_field(tokens[i],acq_count) --;
            add_trace(tokens[i], EVNT_RLS);
        }
    }
}

// TODO: inline cachesize?
// void cache_evict(cache_token_t *token, intptr_t addr) {
//     uint64_t tag = cache_ofst_mask(cache_get_field(token->cache,linesize), addr);
//     uint64_t tag2 = token->tag;
//     token->tag = tag;
//     token_header_field(token,tag) = tag;
//     cache_post(token,CACHE_REQ_EVICT,tag2);
// }

// Assert cache != 0 ?
void * _disagg_alloc(cache_t cache, size_t size) {
    // round to next power of 2
    // TODO: align according to cache line size
    // dprintf("allocating %lu from %u", size, cache);
    static uint64_t start_free = 16;
    intptr_t addr = start_free;
    start_free += size;
    virt_addr_t vaddr = { .cache = cache, .addr = addr };
    return (void *) vaddr.ser;
}

void * _disagg_local_malloc(size_t size) {
    dprintf("allocating %lu from local mem", size);
    intptr_t ofst = local_mr_alloc(size);
    virt_addr_t vaddr = { .cache = 1, .addr = ofst };
    return (void *) vaddr.ser;
}

void * _disagg_stack_alloc(size_t size) {
    dprintf("allocating %lu from stack", size);
    intptr_t ofst = stack_push(size);
    virt_addr_t vaddr = { .cache = 0, .addr = ofst };
    return (void *) vaddr.ser;
}

void _disagg_stack_reclaim(size_t size) {
    return stack_pop(size);
}

void init_cache_stats() {
#ifdef CACHE_LOG_REQ
    printf("Log number of cache requests\n");
#endif
#ifdef CACHE_LOG_ACCESS
    printf("Log number of access\n");
#endif
#ifdef CACHE_LOG_MISS
    printf("Log number of cache misses\n");
#endif
#ifdef CACHE_LOG_LDF
    printf("Log number of late data fetch\n");
#endif
#ifdef CACHE_LOG_PREF
    printf("Log number of prefetches and accurate ones\n");
#endif
#ifdef CACHE_LOG_TRACE
    printf("Record address trace\n");
#endif
    for (int i = 0; i < OPT_NUM_CACHE; ++ i) {
#ifdef CACHE_LOG_TRACE
        // initialize trace
        csts[i].tll.head = new_trace(0, -1);
        csts[i].tll.tail = csts[i].tll.head;
    #endif
    #ifdef CACHE_LOG_MISS
        csts[i].visited = (uint8_t *) malloc(sizeof(uint8_t) * (1 << 25));
#endif
    }
}

void get_cache_logs() {
    printf("----- Cache stats -----\n");
  for (int i = 0; i < cache_cnt; ++ i) {
    printf("  --- Cache %d ---:\n", i);
#ifdef CACHE_LOG_REQ
    printf("total reqs: %lu\n", csts[i].num_reqs);
#endif
#ifdef CACHE_LOG_ACCESS
    printf("total access: %lu\n", csts[i].num_access);
#endif
#ifdef CACHE_LOG_LDF
    printf("access before data arrival: %lu\n", csts[i].late_data_fetch);
#endif
#ifdef CACHE_LOG_MISS
    uint64_t total_miss = csts[i].num_miss;
    printf("total miss: %lu\n", total_miss);
    printf("compulsory miss: %lu %f\n", csts[i].miss_compulsory, (double) csts[i].miss_compulsory / total_miss);
    printf("conflict miss: %lu %f\n", csts[i].miss_conflict, (double) csts[i].miss_conflict / total_miss);
#endif
#ifdef CACHE_LOG_PREF
    printf("total pref: %lu\n", csts[i].num_pref);
    printf("inacc pref: %lu %f\n", csts[i].inacc_pref, (double) csts[i].inacc_pref / csts[i].num_pref);
#endif
#ifdef CACHE_LOG_TRACE
    char fn[32];
    sprintf(fn, "%d-trace.txt", i);
    FILE *trace_i = fopen(fn, "w");
    trace_node *cur = csts[i].tll.head->next;
    while (cur) {
      fprintf(trace_i, "%lu %d\n", cur->addr, cur->event_code);
      cur = cur->next;
    }
    fclose(trace_i);
#endif
  }
}
