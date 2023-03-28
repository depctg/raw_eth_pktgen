#ifndef _RDMAOP_H_
#define _RDMAOP_H_

#include <infiniband/verbs.h>
#include <stdint.h>

#include "common.h"


#define NUM_RDMA_BATCH_WR (32)
#define NUM_RDMA_BATCH_SGE (32 * 4)

extern struct ibv_send_wr _pwr[NUM_RDMA_BATCH_WR];
extern struct ibv_sge _psge[NUM_RDMA_BATCH_SGE];

static inline void build_rdma_wr(int i, uint64_t wr_id,
        uint64_t laddr, uint64_t raddr, int size, ibv_wr_opcode opcode,
        struct ibv_send_wr *next) {
    auto &sge = _psge[i];
    auto &wr = _pwr[i];

    sge.addr = (uint64_t)_rbuf + laddr;
    sge.length = size;
    sge.lkey = mr->lkey;

    wr.wr_id = wr_id;
    wr.sg_list = &sge;
    wr.num_sge = 1;

    wr.wr.rdma.remote_addr = (uint64_t)(peermr.addr) + raddr;
    wr.wr.rdma.rkey = peermr.rkey;
    wr.opcode = opcode;
    wr.next = next;

    dprintf("RDMA Request: [%lx] %lx %s %lx: %d", wr.wr_id, sge.addr,
            opcode == IBV_WR_RDMA_READ ? "<-" : "->",
            wr.wr.rdma.remote_addr, sge.length);
}

#endif
