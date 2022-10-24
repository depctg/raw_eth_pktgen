#ifndef __CLIENT_REMOTE_EXCHANGER_H__
#define __CLIENT_REMOTE_EXCHANGER_H__

#ifdef __cplusplus
extern "C"
{
#endif 

#include "common.h"
#include "app.h"
#include "cache_internal.h"
#include "cache.h"
#include "offload.h"

#define REQ_INFLIGHT 64

RPC_rr_t * req_buf;

static inline void init_exchanger() {
  offload_arg_buf = MEMMAP_CLIENT_REQ;
  req_buf = (RPC_rr_t *) (offload_arg_buf + ARG_BUF_LIMIT);
  offload_ret_buf.data = MEMMAP_CLIENT_RPC_RET;
}

// head, nout not used
// post work will block if is full
static int head = 0;
static int nout = 0;
static inline void _cq_poll() {
    static struct ibv_wc wc[MAX_POLL];

    int n = ibv_poll_cq(cq, MAX_POLL, wc);
    for (int i = 0; i < n; ++ i) {
        if (wc[i].status != IBV_WC_SUCCESS) {
            printf("WR failed\n");
            exit(1);
        }
        if (wc[i].opcode == IBV_WC_RECV) {
            // wr_id = 0 indicates a offload return arrival
            if (!wc[i].wr_id) {
              offload_ret_buf.available = 1;
              continue;
            } 
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
    while (nout >= REQ_INFLIGHT) {
        _cq_poll();
    }

    /* Fill the buf */
    unsigned cur = head;
    head = (head + 1) & (REQ_INFLIGHT - 1);
    nout ++;

    req_buf[cur].op_code = type;
    req_buf[cur].cache_r_header.tag = tag;
    req_buf[cur].cache_r_header.tag2 = tag2;
    req_buf[cur].cache_r_header.cache_id = cache;

    /* Send Packets */
    s_sge[0].addr = (uint64_t)(req_buf + cur);
    s_sge[0].length = sizeof(*req_buf);
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
#else
    UNUSED(ret);
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
#else
        UNUSED(ret);
#endif
    }
}

static inline void rpc_call_post(int function_id, size_t arg_size, size_t ret_size) {
  /* prepare work request */
  struct ibv_sge s_sge[2], r_sge;
  struct ibv_send_wr swr, *bad_wr;
  struct ibv_recv_wr rwr, *bad_rwr;

  dprintf("Calling function id %d", function_id);

  while (nout >= REQ_INFLIGHT) {
    _cq_poll();
  }
  /* Fill the buf */
  unsigned cur = head;
  head = (head + 1) & (REQ_INFLIGHT - 1);
  nout ++;
  req_buf[cur].op_code = FUNC_CALL_BASE;
  req_buf[cur].call_r_header.arg_size = arg_size;
  req_buf[cur].call_r_header.ret_size = ret_size;
  req_buf[cur].call_r_header.procedure_id = function_id;

  /* Send Packets */
  s_sge[0].addr = (uint64_t)(req_buf + cur);
  s_sge[0].length = sizeof(*req_buf);
  s_sge[0].lkey = smr->lkey;

  /* PUSH function arguments */
  s_sge[1].addr = (uint64_t)(offload_arg_buf);
  s_sge[1].length = arg_size;
  s_sge[1].lkey = smr->lkey;

  swr.num_sge = arg_size ? 2 : 1;
  swr.sg_list = s_sge;
  swr.opcode = IBV_WR_SEND;

  swr.wr_id = 0;
  swr.next = NULL;

  swr.send_flags = 0;
#if SEND_CMPL
  swr.send_flags |= IBV_SEND_SIGNALED;
#endif
#if SEND_INLINE
  swr.send_flags |= IBV_SEND_INLINE;
#endif
  int ret = ibv_post_send(qp, &swr, &bad_wr);

#ifndef NDEBUG
  if (unlikely(ret) != 0) {
    fprintf(stderr, "failed in post send\n");
    exit(1);
  }
#else
  UNUSED(ret);
#endif
  if (ret_size) {
    offload_ret_buf.available = 0;
    r_sge.addr = (uint64_t) offload_ret_buf.data;
    r_sge.length = ret_size;
    r_sge.lkey = rmr->lkey;

    rwr.num_sge = 1;
    rwr.wr_id = 0;
    rwr.sg_list = &r_sge;
    rwr.next = NULL;
    ret = ibv_post_recv(qp, &rwr, &bad_rwr);
#ifndef NDEBUG
    if (unlikely(ret) != 0) {
      fprintf(stderr, "failed in post send\n");
      exit(1);
    }
#else
    UNUSED(ret);
#endif
  }
}



#ifdef __cplusplus
}
#endif
#endif 

