#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include "packet.h"

#include "app.h"

static int size_array = 1024;

// local
int job0() {
	int a[size_array];
	int sum = 0;
	for (int i = 0; i < size_array; i++) {
	    sum += a[i];
	}

    printf("SUM %ld\n", sum);
}

// remote: unoptimized
int job1() {
	struct req *reqs = (struct req *)sbuf;
	size_t num_reqs = SEND_BUF_SIZE / sizeof(struct req);
	reqs[0].index = 0;
	reqs[0].size = sizeof(int);
	send(&req[0], sizeof(struct req));

	int a[size_array];
	int sum = 0;
	for (int i = 0; i < size_array; i++) {
	    int ii = i % num_reqs;
	    // send req
	    reqs[ii].index = 0;
	    reqs[ii].size = sizeof(int);
	    send(&req[ii], sizeof(struct req));
	    // recv result
	    recv(&((int *)rbuf)[i], sizeof(int));
	    // get data from buffer
	    sum += ((int *)rbuf)[i];
	}

    printf("SUM %ld\n", sum);
}

// remote: optmized
int job2() {
    int batch_size = sizeof(int) * BATCH_SIZE;
	struct req *reqs = (struct req *)sbuf;
    uint64_t wr_id, wr_id_nxt;

    static const int num_buf = 4;
    int buf_id = 0;

    uint64_t sum = 0;
 
    // Send first request
	reqs[buf_id].index = 0;
	reqs[buf_id].size = batch_size;
	send_async(reqs + buf_id, sizeof(struct req));
	wr_id = recv_async(rbuf, batch_size);

	for (int i = 0; i < ARRAY_SIZE; i += BATCH_SIZE) {
        // Send next request
        int buf_id_nxt = (buf_id + 1) % num_buf;
        if (i + BATCH_SIZE < ARRAY_SIZE) {
            reqs[buf_id_nxt].index = (i + BATCH_SIZE) * sizeof(int);
            reqs[buf_id_nxt].size = batch_size;
            send_async(reqs + buf_id_nxt, sizeof(struct req));
            wr_id_nxt = recv_async((int *)rbuf + buf_id_nxt * BATCH_SIZE, batch_size);
        }

        // printf("wr_id %ld, buf_id %d\n", wr_id, buf_id);
	    // recv result
	    poll(wr_id);
        int * arr = (int *)rbuf + buf_id * BATCH_SIZE;

	    // get data from buffer
        for (int j = 0; j < BATCH_SIZE; j++)
            sum += arr[j];

        // prepare for next poll
        wr_id = wr_id_nxt;
        buf_id = buf_id_nxt;
	}

    printf("SUM %ld\n", sum);
}

static int n_jobs = 3;
static void (*jobs[n_jobs]) () = {job0, job1, job2};

// addr(tcp://*:3456) job size n_runs
int main(int argc, char * argv[]) {
    if (argc < 2) return -1;

	init(TRANS_TYPE_RC, argv[1]);

    int job, n_runs = 10;
    sscanf (argv[2], "%d", &job);
    if (job >= n_jobs) return -1;
    if (argc > 3) sscanf (argv[3], "%d", &size_array);
    if (argc > 4) sscanf (argv[4], "%d", &n_runs);

    uint64_t totalNs = 0; // can overflow
    void (*f)() = jobs[job];
    for (int i = 0; i < n_runs; ++i) {
        uint64_t startNs = getCurNs();
        (*f)();
        uint64_t endNs = getCurNs();
        totalNs += endNs - startNs;
    }
    printf("n_runs: %d, avg ns: %ld \n", n_runs, totalNs / n_runs)

    return 0;
}

