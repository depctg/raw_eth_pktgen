#include <stdio.h>
#include <infiniband/verbs.h>
#include <unistd.h>

#include "common.h"
#include "packet.h"
#include "app.h"

static unsigned int u_sleep = 100;

static inline int app_init() {
    // generate c array in sbuf
	int * a = (int *)sbuf;
    unsigned long long sum = 0;
	// init a
	for (int i = 0; i < ARRAY_SIZE; i++) {
		a[i] = i;
        sum += a[i];
    }

    printf("Local sum %lld\n", sum);

    return 0;
}

// non-zero copy version
void job0() {
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
	    usleep(u_sleep);
	    send(&reps[0], sizeof(int));
	}
}

void job1() {
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
                usleep(u_sleep);
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
}

const static int n_jobs = 2;
static void (*jobs[2]) () = {job0, job1};

// addr job usleep
int main(int argc, char * argv[]) {
    if (argc < 2) return -1;
 
    int job;
    sscanf (argv[2], "%d", &job);
    if (job >= n_jobs) return -1;
    if (argc > 2) sscanf (argv[2], "%u", &u_sleep);

	init(TRANS_TYPE_RC_SERVER, argv[1]);
    app_init();
    printf("start processing requests...\n");

    (*jobs[job])();

    return 0;
}

