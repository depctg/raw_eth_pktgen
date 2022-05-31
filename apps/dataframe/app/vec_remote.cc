#include <iostream>
#include "common.h"
#include "cache.h"
#include <assert.h>
#include <string>
#include <getopt.h>

#define CACHE_LINE_LIMIT 1024 * 1024

struct cache_req_full {
    struct cache_req r;
    uint8_t data[CACHE_LINE_LIMIT];
};

#define REQ_TYPE struct cache_req_full

static size_t cache_line_size = 8192;

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

static struct option long_options[] = {
    {"addr", required_argument, 0, 0},
    // {"cache_size", required_argument, 0, 0},
    {"cache_line_size", required_argument, 0, 0},
    {0, 0, 0, 0}
};

int main(int argc, char * argv[]) 
{
    char * addr = 0;
    // uint64_t cache_size = (480 << 20);

    int opt= 0, long_index =0;
    while ((opt = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1) {
        switch (long_index) {
            case 0:
                addr = optarg;
                break;
            case 1:
                cache_line_size = atoll(optarg);
                break;
            case 2:
                break;
            case 3:
                break;
            default:
                return -1;
        }
    }
    if (!addr) return -1;

    init(TRANS_TYPE_RC_SERVER, addr);

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
