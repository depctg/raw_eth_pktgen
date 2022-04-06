#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
 
#include "common.h"
#include "packet.h"

#include "app.h"

static int size_array = 1024;

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
// ARRAY_SIZE BATCH_SIZE in app.h
void job2() {
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

// addr(tcp://localhost:3456) job size n_runs
int main(int argc, char * argv[]) {
    if (argc < 2) return -1;

    init(TRANS_TYPE_RC, argv[1]);

    printf("init done\n");

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
        printf("n_run: %d, ns: %ld \n", i, endNs - startNs);
    }
    printf("n_runs: %d, avg ns: %ld \n", n_runs, totalNs / n_runs);

    return 0;
}
