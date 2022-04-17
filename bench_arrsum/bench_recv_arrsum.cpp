#include <stdio.h>
#include <stdlib.h>
#include <infiniband/verbs.h>
#include <unistd.h>
#include <getopt.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

#include "../common.h"
#include "../packet.h"
#include "../app.h"

using namespace std;
// default values
static unsigned int nanoscs = 100;
static int size_array = 1024;

void rdma_latency(unsigned int d) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(d));
}

// generate c array in sbuf
static inline int app_init() {
	int * a = (int *)sbuf;
    unsigned long long sum = 0;
	// init a in sbuf
	for (int i = 0; i < size_array; i++) {
		a[i] = i;
        sum += a[i];
    }

    printf("Local sum %lld\n", sum);

    return 0;
}

// non-zero copy version
// not using app_init(), which initializes the sbuf
void job0() {
	const int size = 1024;
	int a[size];
	struct req *reqs = (struct req *)rbuf;
	//size_t num_reqs = RECV_BUF_SIZE / sizeof(struct req);
	int * reps = (int *)sbuf;
	while (1) {
	    recv(&reqs[0], sizeof(struct req));
	    // get datra
	    // Data copy!
	    reps[0] = a[reqs[0].index];
	    // sleep
        if (nanoscs) rdma_latency(nanoscs);
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
                if (nanoscs) rdma_latency(nanoscs);
                // cout << u_sleep;
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
}

// orders match
static void (*jobs[]) () = {job0, job1};
static std::string jobs_desc[] = {"naive", "zero copy"};
static struct option long_options[] = {
    {"addr", required_argument, 0, 0},
    {"job", required_argument, 0, 0},
    {"nsleep", required_argument, 0, 0},
    {"size_array", required_argument, 0, 0},
    {0, 0, 0, 0}
};

int main(int argc, char * argv[]) {
    char * addr = 0;
    int job = -1;

    int opt= 0, long_index =0;
    while ((opt = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1) {
        switch (long_index) {
            case 0:
                addr = optarg;
                break;
            case 1:
                job = atoi(optarg);
                break;
            case 2:
                nanoscs = atoi(optarg);
                break;
            case 3:
                size_array = atoi(optarg);
                break;
             default:
                return -1;
        }
    }

    if (!addr) return -1;
    if (job == -1 || job >= sizeof(jobs) / sizeof(jobs[0])) return -1;

	init(TRANS_TYPE_RC_SERVER, addr);
    app_init();

    printf("start processing requests...\n");
    printf("ctrl c to stop after each benchmark\n");

    printf("running: %s\n", jobs_desc[job].c_str());
    (*jobs[job])();

    return 0;
}
