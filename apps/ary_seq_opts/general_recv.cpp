#include "../common.h"
#include "../packet.h"
#include "../app.h"
#include <infiniband/verbs.h>
#include <chrono>
#include <iostream>
#include <string.h>

using namespace std;
static inline int app_init() {
    // generate c array in sbuf
	int * a = (int *)sbuf;
    unsigned long long sum = 0;
	// init a
	for (int i = 0; i < ARRAY_SIZE; i++) {
		*a = i;
        sum += *a;
        a++;
    }

    cout <<("Local sum %lld\n", sum) << endl;

    return 0;
}

int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC_SERVER, argv[1]);
    app_init();
    printf("start processing requests...\n");

/*  non-zero copy version
	const int size = 1024;
	int a[size];
	int sum = 0;
	struct req *reqs = (struct req *)rbuf;
	size_t num_reqs = RECV_BUF_SIZE / sizeof(struct req);
	int * reps = (int *)sbuf;
	while (1) {
	    recv(&reqs[0], sizeof(struct req));
	    // get datra
	    // Data copy!
	    reps[0] = a[reqs[0].index];
	    // sleep
	    // usleep();
	    send(&reps[0], sizeof(int));
	}
*/

    const unsigned int max_recvs = 64;
    const unsigned int inflights = max_recvs / 2;
	struct ibv_wc wc[max_recvs];

    unsigned int post_recvs = 0, poll_recvs = 0;
	struct req *reqs = (struct req *)rbuf;

    // First, we post multiple requests
    for (int i = 0; i < inflights; i++)
        recv_async(reqs + i, sizeof(struct req));
    post_recvs += inflights;

	while (1) {
        // here we do not want to poll by id, just call ibv_xxx
        int n = ibv_poll_cq(cq, inflights, wc);

        // process the requests
        for (int i = 0; i < n; i++) {
            // not a timeout
            if (wc[i].status == 0 && wc[i].wr_id != 0) {
                int idx = (poll_recvs++) % max_recvs;

                // process request
                // sleep here to change the latency
                usleep(0);
                // cout << "receive req: idx " << reqs[idx].index << " size: " << reqs[idx].size << endl;
                send_async((char *)sbuf + reqs[idx].index, reqs[idx].size);
            }
        }

        // fill the recv queue
        while (post_recvs < poll_recvs + inflights) {
            int idx = post_recvs % max_recvs;
            recv_async(reqs + idx, sizeof(struct req));
            post_recvs ++;
        }
	}
    
    return 0;

}

