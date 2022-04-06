#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
 
#include "common.h"
#include "packet.h"

#include "app.h"

// default values
static int size_array = 1024;
static int size_batch = 64;
static int pre_stride = 4;

// local
void job0() {
    int a[size_array];
    int sum = 0;
    for (int i = 0; i < size_array; i++) {
        sum += a[i];
    }

    printf("SUM %ld\n", sum);
}

// remote: unoptimized
void job1() {
    struct req *reqs = (struct req *)sbuf;
    size_t num_reqs = SEND_BUF_SIZE / sizeof(struct req);
    reqs[0].index = 0;
    reqs[0].size = sizeof(int);
    send(&reqs[0], sizeof(struct req));

    int sum = 0;
    for (int i = 0; i < size_array; i++) {
        // send req
        reqs[0].index = i * sizeof(int);
        reqs[0].size = sizeof(int);
        send(&reqs[0], sizeof(struct req));
        // recv result
        recv(rbuf, sizeof(int));
        // get data from buffer
        sum += ((int *)rbuf)[0];
    }

    printf("SUM %ld\n", sum);
}

// remote: optmized
// prefetch one batch
// todo: plot runtime over batch_size + latency
void job2() {
    int batch_size = sizeof(int) * size_batch;
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

    for (int i = 0; i < size_array; i += size_batch) {
        // Send next request
        int buf_id_nxt = (buf_id + 1) % num_buf;
        if (i + size_batch < size_array) {
            reqs[buf_id_nxt].index = (i + size_batch) * sizeof(int);
            reqs[buf_id_nxt].size = batch_size;
            send_async(reqs + buf_id_nxt, sizeof(struct req));
            wr_id_nxt = recv_async((int *)rbuf + buf_id_nxt * size_batch, batch_size);
        }

        // printf("wr_id %ld, buf_id %d\n", wr_id, buf_id);
        // recv result
        poll(wr_id);
        int * arr = (int *)rbuf + buf_id * size_batch;

        // get data from buffer
        for (int j = 0; j < size_batch; j++)
            sum += arr[j];

        // prepare for next poll
        wr_id = wr_id_nxt;
        buf_id = buf_id_nxt;
    }

    printf("SUM %ld\n", sum);
}

// remote: optimized
// tiling
void job3() {

}

void swap (int *a, int *b) {
    int temp = *a; *a = *b; *b = temp;
}
 
// initialize a random access pattern for all elements
// remember to free the result
int* init_access_patten(int n) {
    int* a = (int *)malloc(sizeof(int)*n); 
    srand(time(NULL));

    for (int i = 0; i < n; ++i) {
        a[i] = i;
    }
 
    for (int i = n-1; i > 0; i--) {
        swap(&a[i], &a[rand() % (i+1)]);
    }

    return a;
}

// local: random access
void job4() {
    int a[size_array];
    int sum = 0;
    int* pattern = init_access_patten(size_array);
    for (int i = 0; i < size_array; i++) {
        sum += a[pattern[i]];
    }

    printf("SUM %ld\n", sum);
}

const static int n_jobs = 3;
static void (*jobs[3]) () = {job0, job1, job2};
// cosmetic
static char jobs_desc[3]  = {"local sequential", "remote sequential", "remote sequential prefetch=1"};
static struct option long_options[] = {
    {"addr", required_argument, 0, 0},
    {"job", required_argument, 0, 0},
    {"array_size", required_argument, 0, 0},
    {"n_runs", required_argument, 0, 0},
    {"size_batch", required_argument, 0, 0},
    {"pre_stride", required_argument, 0, 0},
    {0, 0, 0, 0}
};

int main(int argc, char * argv[]) {
    char * addr = 0; // e.g. tcp://localhost:3456
    int job = -1, n_runs = 10;

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
                size_array = atoi(optarg);
                break;
            case 3:
                n_runs = atoi(optarg);
                break;
            case 4:
                size_batch = atoi(optarg);
                break;
            case 5:
                pre_stride = atoi(optarg);
                break;
             default:
                return -1;
        }
    }

    if (!addr) return -1;
    if (job == -1 || job >= n_jobs) return -1;

    init(TRANS_TYPE_RC, argv[1]);
    printf("init done\n");

    uint64_t totalNs = 0; // can overflow
    void (*f)() = jobs[job];
    printf("running: %s\n", jobs_desc[job]);
    for (int i = 0; i < n_runs; ++i) {
        uint64_t startNs = getCurNs();
        (*f)();
        uint64_t endNs = getCurNs();
        totalNs += endNs - startNs;
        printf("n_run: %d, ns: %ld \n", i, endNs - startNs);
    }

    printf("n_runs: %d, avg ns: %ld \n", n_runs, totalNs / n_runs);

    return 0;
}

