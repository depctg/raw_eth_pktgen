#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include "robj.h"

typedef struct local_send_remote_recv {
  remote_msg_header header; // will always be in sbuf
  uint8_t data[payload_Bsize]; // source buf may be from outside
} local_send_remote_recv;

typedef struct local_recv_remote_send {
  struct local_msg_header header;
  uint8_t data[payload_Bsize];
} local_recv_remote_send;

static local_send_remote_recv *lsrr;
static local_recv_remote_send *lrrs;
static int head = 0, tail = 0, posts = 0, polls = 0;

static inline void commu_post(int lsrr_idx, uint64_t buf_addr, uint8_t need_copy) {
  int ret;
  struct ibv_sge sge[2], rsge;
  struct ibv_send_wr wr, *bad_wr;
  struct ibv_recv_wr rwr, *bad_rwr;

  remote_msg_header *header = &(lsrr[lsrr_idx].header);
  void *data = &(lsrr[lsrr_idx].data);
  dprintf("remote_header: %lu, %lu, %d, %lu", 
    header->obj_id, 
    header->offset, 
    header->type, 
    header->p_Bsize);

  /* Set send header */
  sge[0].addr = (uint64_t) header;
  sge[0].length = sizeof(remote_msg_header);
  sge[0].lkey = smr->lkey;

  /* Set send payload */
  /* Not in sge list if is read request */
  uint64_t sge_addr = buf_addr;
  if (need_copy) {
    memcpy(data, (void *) buf_addr, header->p_Bsize);
    sge_addr = (uint64_t) data;
  }
  sge[1].addr = sge_addr;
  sge[1].length = payload_Bsize;
  sge[1].lkey = rmr->lkey;

  wr.num_sge = header->type == REQ_WRITE ? 1 : 2;
  wr.sg_list = &sge[0];
  wr.opcode = IBV_WR_SEND;
  wr.next = NULL;

  wr.send_flags = 0;
#if SEND_CMPL
    wr.send_flags |= IBV_SEND_SIGNALED;
#endif
#if SEND_INLINE
  if (payload_Bsize < 512)
    wr.send_flags |= IBV_SEND_INLINE;
#endif

  ret = ibv_post_send(qp, &wr, &bad_wr);
#ifndef NDEBUG
  if (unlikely(ret) != 0) {
      fprintf(stderr, "failed in post send\n");
      exit(1);
  }
#endif

  // if need reply
  if (header->type == REQ_READ) {
    if (need_copy) {
      rsge.addr = (uint64_t) data;
      rwr.wr_id = buf_addr; 
    } else {
      rsge.addr = buf_addr;
      rwr.wr_id = 0; 
    }
    rsge.length = payload_Bsize;
    rsge.lkey = rmr->lkey;

    rwr.num_sge = 1;
    rwr.sg_list = &rsge;
    
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

static inline void commu_poll();

#define _nullptr_t 0

void init_manager(size_t *dss, void *lsrr, void *lrrs) {
  DS_LIST = dss; 
  lsrr = lsrr;
  lrrs = lrrs;
}

#define ptr_set_flag(ptr, flag) \
  ((ptr.flags) |= (flag))
#define ptr_unset_flag(ptr, flag) \
  ((ptr.flags) &= (~(flag)))
#define ptr_check_flag(ptr, flag) \
  ((ptr.flags) & (flag))
#define ceil(size, line) \
  (1 + (((size)-1) / (line)))

static uint64_t obj_id = 0;

disagg_ptr alloc_obj_n(uint8_t ds_id, uint64_t n) {
  disagg_ptr ptr;
  ptr.ds_id = ds_id;
  ptr.mem_size = n * DS_LIST[ds_id];
  ptr.obj_id = obj_id ++; 
  ptr.flags = 0;
  ptr.status = 0;
  ptr.addr = _nullptr_t;
  void *mem = malloc(ptr.mem_size);
  ptr.addr = (uint64_t) mem;
  ptr_set_flag(ptr, VALID);
  ptr_set_flag(ptr, PRESENCE);
  // inspect_ptr(&ptr);
  return ptr;
}

void inspect_ptr(disagg_ptr *ptr) { 
  dprintf("PTR info: %lu, %lu, %lu, %d, %d, %d", ptr->obj_id, ptr->addr, ptr->mem_size, ptr->ds_id, ptr->status, ptr->flags);
}

void dealloc(disagg_ptr *ptr) {
  if (!ptr_check_flag((*ptr), VALID)) return;
  if (!ptr_check_flag((*ptr), PRESENCE) && ptr->status == SYNC) {
    await(ptr);
  }   
  free((void *)ptr->addr);
  ptr->addr = _nullptr_t;
  ptr->mem_size = 0;
  ptr_unset_flag((*ptr), VALID);
  ptr_unset_flag((*ptr), PRESENCE);
}

void *lookup_ptr(disagg_ptr *ptr) {
  if (!ptr_check_flag((*ptr), VALID)) {
    printf("Can't deref freed object\n");
    exit(1);
  } 
  if (ptr_check_flag((*ptr), PRESENCE)) {
    return (void *) (ptr->addr);
  } else if (ptr->status == IDLE) {
    prefetch(ptr);
    await(ptr);
  } else {
    await(ptr);
  }
  return (void *) (ptr->addr);
};

void evict(disagg_ptr *ptr) {
  if (!ptr_check_flag((*ptr), VALID)) {
    printf("Can't evict freed object\n");
    exit(1);
  }
  if (!ptr_check_flag((*ptr), PRESENCE)) {
    if (ptr->status == SYNC) await(ptr);
  } else {
    uint64_t num_wrs = ceil(ptr->mem_size, payload_Bsize);
    for (uint64_t i = 0; i < num_wrs; ++ i) {
      int lsrr_idx = head;
      head = (head + 1) % max_inflight;
      uint64_t offset = i * payload_Bsize;
      lsrr[lsrr_idx].header.obj_id = ptr->obj_id;
      lsrr[lsrr_idx].header.offset = offset;
      lsrr[lsrr_idx].header.type = REQ_WRITE;
      lsrr[lsrr_idx].header.p_Bsize = ((i < num_wrs-1) ? payload_Bsize : ptr->mem_size - payload_Bsize * i);
    }
  }
}

void prefetch(disagg_ptr *ptr){}
void await(disagg_ptr *ptr){}
void sync_obj(disagg_ptr *ptr){}