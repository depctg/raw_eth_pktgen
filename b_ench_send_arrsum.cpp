#include <string.h>
#include <time.h>
#include <queue>
#include <iostream>
#include "common.h"
#include "packet.h"
#include "app.h"

using namespace std;
static int size_array = 1024;

// local
void job0() {
    int *a = new int[size_array];
    unsigned long long sum = 0;
    for (int i = 0; i < size_array; i++) {
        *(a+i) = i;
        sum += *(a+i);
    }
    delete[] a;
    printf("SUM %ld\n", sum);
}

// Complete fetch
void job1() {
    struct req *reqs = (struct req*) sbuf;
    unsigned long long sum = 0;
    // fetch the entire array back
    int ts = sizeof(int) * size_array;
    reqs[0].index = 0;
    reqs[0].size = ts;
    send(reqs, sizeof(struct req));
    recv(rbuf, ts);
    int *a = (int*) rbuf;
    // cout << a[size_array] << endl;
    for (int i = 0; i < size_array; ++i) {
        sum += *a++;
    }
    printf("SUM %ld\n", sum);
}

// remote: unoptimized
void job2() {
    struct req *reqs = (struct req *)sbuf;

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

// remote: optimized
// batched fetch
void job3() {
    // batch optimization
    int batch_size = sizeof(int) * BATCH_SIZE;
	struct req *reqs = (struct req *)sbuf;

    uint64_t sum = 0;
    // Send first request
    int buf_ofst = 0;
	reqs[0].index = 0;
	reqs[0].size = batch_size;
	for (int i = 0; i < ARRAY_SIZE; i += BATCH_SIZE) {
	    // fetch batch
        send(reqs, sizeof(struct req));
        recv((int*)rbuf + buf_ofst*BATCH_SIZE, batch_size);
        int * arr = (int *)rbuf + buf_ofst * BATCH_SIZE;

	    // get data from buffer
        for (int j = 0; j < BATCH_SIZE; j++)
            sum += *arr++;
        buf_ofst ++;
        reqs[0].index = buf_ofst * batch_size;
        reqs[0].size = batch_size;
	}
    printf("SUM %ld\n", sum);
}

// remote: optmized
// prefetch STRIDE batches
// todo: plot runtime over batch_size + latency
// ARRAY_SIZE BATCH_SIZE in app.h
void job4() {
    // strided_prefetch
    int batch_size = sizeof(int) * BATCH_SIZE;
	struct req *reqs = (struct req *)sbuf;
    uint64_t wr_id_cur;
    queue<pair<uint64_t, int>> wr_ids;

    const int num_buf = 2 * PRE_STRIDE;
    int req_step_id = 0;

    uint64_t sum = 0;
    int max_steps = ARRAY_SIZE / BATCH_SIZE;

    // Pre-pre-fetch   
    int step_further = min(PRE_STRIDE, max_steps);
    for (int i = 0; i < step_further; ++i) {
        int idx = (req_step_id + i) % num_buf;
        // cout << "req " << req_step_id + i << "idx " << idx << endl;
        reqs[idx].index = i * BATCH_SIZE * sizeof(int);
        reqs[idx].size = batch_size;
        send_async(reqs + idx, sizeof(struct req));
        wr_id_cur = recv_async((int *)rbuf + idx * BATCH_SIZE, batch_size);
        wr_ids.push({wr_id_cur, idx});
    }

    req_step_id += step_further;
    for (int mut_step_id = 0; mut_step_id < max_steps; ++mut_step_id) {
        // pre-fetch next round
        if (mut_step_id % PRE_STRIDE == PRE_STRIDE / 2) {
            int step_further = min(PRE_STRIDE, max_steps - req_step_id);
            for (int i = 0; i < step_further; ++i) {
                int idx = (req_step_id + i) % num_buf;
                // cout << "req " << req_step_id + i << "idx " << idx << endl;
                reqs[idx].index = (req_step_id + i) * BATCH_SIZE * sizeof(int);
                reqs[idx].size = batch_size;
                send_async(reqs + idx, sizeof(struct req));
                wr_id_cur = recv_async((int *)rbuf + idx * BATCH_SIZE, batch_size);
                wr_ids.push({wr_id_cur, idx});
            }
            req_step_id += step_further;
        }
        // cout << "Requested up to: " << req_step_id << endl;
        pair<uint64_t, int> p = wr_ids.front();
        poll(p.first);
        int * arr = (int *)rbuf + p.second * BATCH_SIZE;
        wr_ids.pop();

	    // get data from buffer
        for (int j = 0; j < BATCH_SIZE; j++)
            sum += arr[j];
    }

    printf("SUM %ld\n", sum);
}

// remote: optimized
// tiling
void job5() {

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
void job6() {
    int a[size_array];
    int sum = 0;
    int* pattern = init_access_patten(size_array);
    for (int i = 0; i < size_array; i++) {
        sum += a[pattern[i]];
    }

    printf("SUM %ld\n", sum);
}

const static int n_jobs = 7;
static void (*jobs[5]) () = {job0, job1, job2, job3, job4};

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

