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
  init_server();
	// init remote server mem
	manager_init();

  const int inflights = MAX_POLL / 2;
	struct ibv_wc wc[MAX_POLL];

  unsigned int post_recvs = 0, poll_recvs = 0;
	RPC_rrf_t *req_fulls = (RPC_rrf_t *) rbuf;

  // First, we post multiple requests
  for (int i = 0; i < inflights; i++)
    recv_async(req_fulls + i, sizeof(*req_fulls));
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
        if (req_fulls[idx].rr.op_code < FUNC_CALL_BASE) 
          process_cache_req(req_fulls + idx);
        else
          process_call_req(req_fulls + idx);
      }
    }
    // fill the recv queue
    while (post_recvs < poll_recvs + inflights) {
      int idx = post_recvs % MAX_POLL;
      recv_async(req_fulls + idx, sizeof(*req_fulls));
      post_recvs ++;
    }
	}
	return 0;
}