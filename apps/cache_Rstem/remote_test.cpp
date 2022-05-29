#include "common.h"
#include "cache.h"
#include <assert.h>

#define CACHE_LINE_LIMIT 1024 * 1024

struct cache_req_full {
    struct cache_req r;
    uint8_t data[CACHE_LINE_LIMIT];
};

#define REQ_TYPE struct cache_req_full

size_t cache_line_size;

static inline void process_req(REQ_TYPE * req) {
    if (req->r.type == CACHE_REQ_READ || req->r.type == CACHE_REQ_EVICT) {
        uint64_t offset = req->r.tag & CACHE_TAG_MASK;
        dprintf("READ: offset %lx, line %d", offset, cache_line_size);
        send_async((char *)sbuf + offset, cache_line_size);
    }
    // write
    if (req->r.type == CACHE_REQ_WRITE) {
        uint64_t offset = req->r.tag & CACHE_TAG_MASK;
        dprintf("WRITE: offset %lx, line %d", offset, cache_line_size);
        memcpy((char *)sbuf + offset, req->data, cache_line_size);
    } else if (req->r.type == CACHE_REQ_EVICT) {
        uint64_t offset = req->r.newtag & CACHE_TAG_MASK;
        dprintf("EVICT: offset %lx, line %d", offset, cache_line_size);
        memcpy((char *)sbuf + offset, req->data, cache_line_size);
    }
}

int main(int argc, char * argv[]) {
    init(TRANS_TYPE_RC_SERVER, argv[1]);
    cache_line_size = 1 << 8;

    const int max_recvs = 64;
    const int inflights = max_recvs / 2;
	struct ibv_wc wc[max_recvs];

    unsigned int post_recvs = 0, poll_recvs = 0;
	REQ_TYPE *reqs = (REQ_TYPE *)rbuf;
    char * data = (char *)sbuf;

    // First, we post multiple requests
    for (int i = 0; i < inflights; i++)
        recv_async(reqs + i, sizeof(REQ_TYPE));
    post_recvs += inflights;

	while (1) {
        // here we do not want to poll by id, just call ibv_xxx
        int n = poll_cq(cq, inflights, wc);

        // process the requests
        for (int i = 0; i < n; i++) {
            // not a timeout
            if (wc[i].status == IBV_WC_SUCCESS && wc[i].opcode == IBV_WC_RECV) {
                int idx = (poll_recvs++) % max_recvs;

                // process request
                // sleep here to change the latency
                process_req(reqs + idx);
            }
        }

        // fill the recv queue
        while (post_recvs < poll_recvs + inflights) {
            int idx = post_recvs % max_recvs;
            recv_async(reqs + idx, sizeof(REQ_TYPE));
            post_recvs ++;
        }
	}
    
    return 0;
}
