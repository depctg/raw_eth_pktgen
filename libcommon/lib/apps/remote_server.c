#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "remote_pool.h"
#include "cache.h"
#include "helper.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage remote_server [url]");
  }
  init(TRANS_TYPE_RC_SERVER, argv[1]);

	// init remote server mem
	manager_init(sbuf);

	/* naive 1c */
	const uint64_t node_cls = align_with_pow2(16);
	add_pool(2, node_cls);

  const int inflights = MAX_POLL / 2;
	struct ibv_wc wc[MAX_POLL];

  unsigned int post_recvs = 0, poll_recvs = 0;
	R_REQ_TYPE *reqs = (R_REQ_TYPE *) rbuf;

  // First, we post multiple requests
  for (int i = 0; i < inflights; i++)
    recv_async(reqs + i, sizeof(R_REQ_TYPE));
  post_recvs += inflights;

	while (1) {
    // here we do not want to poll by id, just call ibv_xxx
    int n = ibv_poll_cq(cq, MAX_POLL, wc);

    // process the requests
    for (int i = 0; i < n; i++) {
      // not a timeout
      if (wc[i].status == IBV_WC_SUCCESS && wc[i].opcode == IBV_WC_RECV) {
        int idx = (poll_recvs++) % MAX_POLL;

        // process request
        // sleep here to change the latency
        process_req(reqs + idx);
      }
    }
    // fill the recv queue
    while (post_recvs < poll_recvs + inflights) {
      int idx = post_recvs % MAX_POLL;
      recv_async(reqs + idx, sizeof(R_REQ_TYPE));
      post_recvs ++;
    }
	}
	return 0;
}