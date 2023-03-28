#include <infiniband/verbs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "queue.h"
#include "cache.hpp"
#include "rdmaop.hpp"

/*
 * RDMA related functions
 *
 * Here we explain the structure of wrid
 * | options : 8 | qid : 8 | seq : 16 | tag : 32 |
 */

#define get_id(wr,i) ((struct _wr_id *)&(wr[i].wr_id))

struct ibv_send_wr _pwr[NUM_RDMA_BATCH_WR];
struct ibv_sge _psge[NUM_RDMA_BATCH_SGE];

// Send and recv queues
struct queue_info qi[NUM_QUEUES];

// need to include all metadata types
// register checking method for different types of cache!
static void inline meta_udpate(uint8_t qid, uint8_t cls, uint16_t seq, uint32_t tag) {
    switch (cls) {
    }
}

// Put check out of loop
// TODO: consider template
// seq: target sequence number
void poll(uint16_t qid, uint16_t seq) {
    struct ibv_wc wc[MAX_POLL];
    // TODO: inflight?
    // test this!
    while (qi[qid].rid - seq > MAX_QUEUE_INFLIGHT) {
        int n = ibv_poll_cq(cq, MAX_POLL, wc);
        for (int i = 0; i < n; i++) {
            /* if requires an queue update */
            if (!(wc[i].wr_id & REQWR_OPT_QUEUE_IGNORE) &&
                get_id(wc,i)->seq - qi[get_id(wc,i)->qid].rid < MAX_QUEUE_INFLIGHT) {
                qi[get_id(wc,i)->qid].rid = get_id(wc,i)->seq;
            }
#if 0
            /* if requires an meta update */
            if (wc[i].wr_id & REQWR_OPT_META_UPDATE) {
                // we won't do any meta update for now
            }
#endif
        }
    }
}

