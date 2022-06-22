#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "util_disagg.h"
#include "common.h"
#include "remote_pool.h"

typedef struct cache_req_full REQ_TYPE;

int main(int argc, char * argv[]) {
    init(TRANS_TYPE_RC_SERVER, argv[1]);
    manager_init(sbuf);
    const uint64_t graph_node_cls = align_with_pow2(sizeof(GraphNode) * 16);
    const uint64_t heap_node_cls = align_with_pow2(sizeof(MinHeapNode) * 8);
    add_pool(0, graph_node_cls);
    add_pool(1, heap_node_cls);
    // add_pool(0, heap_node_cls);

    const int max_recvs = 64;
    const int inflights = max_recvs / 2;
	struct ibv_wc wc[max_recvs];

    unsigned int post_recvs = 0, poll_recvs = 0;
	REQ_TYPE *reqs = (REQ_TYPE *)rbuf;

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
