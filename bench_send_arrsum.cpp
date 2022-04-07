#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <queue>
// #include <chrono>
#include <string>
 
#include "common.h"
#include "packet.h"
#include "app.h"

using namespace std;

// default values
static int size_array = 1024;
static int size_batch = 64;
static int pre_stride = 4;

// local
void job0() {
    int a[size_array];
    long sum = 0;
    for (int i = 0; i < size_array; i++) {
        a[i] = i;
        sum += a[i];
    }

    printf("SUM %ld\n", sum);
}

// remote: unoptimized
void job1() {
    struct req *reqs = (struct req *)sbuf;
    // size_t num_reqs = SEND_BUF_SIZE / sizeof(struct req);
    // reqs[0].index = 0;
    // reqs[0].size = sizeof(int);
    // send(&reqs[0], sizeof(struct req));

    long sum = 0;
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

// remote: completed fetch
void job3() {
    struct req *reqs = (struct req*) sbuf;
    long sum = 0;
    // fetch the entire array back
    int ts = sizeof(int) * size_array;
    reqs[0].index = 0;
    reqs[0].size = ts;
    send(reqs, sizeof(struct req));
    recv(rbuf, ts);
    int *a = (int*) rbuf;
    for (int i = 0; i < size_array; ++i) {
        sum += a[i];
    }
    printf("SUM %ld\n", sum);
}

void job_batched_fetch() {
    // batch optimization
    int batch_size = sizeof(int) * size_batch;
	struct req *reqs = (struct req *)sbuf;

    uint64_t sum = 0;
    // Send first request
    int buf_ofst = 0;
	reqs[0].index = 0;
	reqs[0].size = batch_size;
	for (int i = 0; i < size_array; i += size_batch) {
	    // fetch batch
        send(reqs, sizeof(struct req));
        recv((int*)rbuf + buf_ofst*size_batch, batch_size);
        int * arr = (int *)rbuf + buf_ofst * size_batch;

	    // get data from buffer
        for (int j = 0; j < size_batch; j++)
            sum += *arr++;
        buf_ofst ++;
        reqs[0].index = buf_ofst * batch_size;
        reqs[0].size = batch_size;
	}
    printf("SUM %ld\n", sum);
}

void job_stride_batched_fetch() {
    // strided_prefetch
    int batch_size = sizeof(int) * size_batch;
	struct req *reqs = (struct req *)sbuf;
    uint64_t wr_id_cur;
    queue<pair<uint64_t, int>> wr_ids;

    const int num_buf = 2 * pre_stride;
    int req_step_id = 0;

    uint64_t sum = 0;
    int max_steps = size_array / size_batch;

    // Pre-pre-fetch   
    int step_further = min(pre_stride, max_steps);
    for (int i = 0; i < step_further; ++i) {
        int idx = (req_step_id + i) % num_buf;
        // cout << "req " << req_step_id + i << "idx " << idx << endl;
        reqs[idx].index = i * size_batch * sizeof(int);
        reqs[idx].size = batch_size;
        send_async(reqs + idx, sizeof(struct req));
        wr_id_cur = recv_async((int *)rbuf + idx * size_batch, batch_size);
        wr_ids.push({wr_id_cur, idx});
    }

    req_step_id += step_further;
    for (int mut_step_id = 0; mut_step_id < max_steps; ++mut_step_id) {
        // pre-fetch next round
        if (mut_step_id % pre_stride == pre_stride / 2) {
            int step_further = min(pre_stride, max_steps - req_step_id);
            for (int i = 0; i < step_further; ++i) {
                int idx = (req_step_id + i) % num_buf;
                // cout << "req " << req_step_id + i << "idx " << idx << endl;
                reqs[idx].index = (req_step_id + i) * size_batch * sizeof(int);
                reqs[idx].size = batch_size;
                send_async(reqs + idx, sizeof(struct req));
                wr_id_cur = recv_async((int *)rbuf + idx * size_batch, batch_size);
                wr_ids.push({wr_id_cur, idx});
            }
            req_step_id += step_further;
        }
        // cout << "Requested up to: " << req_step_id << endl;
        pair<uint64_t, int> p = wr_ids.front();
        poll(p.first);
        int * arr = (int *)rbuf + p.second * size_batch;
        wr_ids.pop();

	    // get data from buffer
        for (int j = 0; j < size_batch; j++)
            sum += arr[j];
    }

    printf("SUM %ld\n", sum);
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
    long sum = 0;
    int* pattern = init_access_patten(size_array);
    for (int i = 0; i < size_array; i++) {
        sum += a[pattern[i]];
    }
    free(pattern);

    printf("SUM %ld\n", sum);
}

// orders match
static void (*jobs[]) () = {job0, job1, job2, job3, job_batched_fetch, job_stride_batched_fetch, job4};
static std::string jobs_desc[] = {"local sequential", "remote sequential", "remote sequential prefetch=1",
    "remote sequential prefetch=all", "remote sequential batch prefetch",
    "remote sequential strided batch prefetch", "local random"};
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
    if (job == -1 || job >= sizeof(jobs) / sizeof(jobs[0])) return -1;

    init(TRANS_TYPE_RC, addr);
    printf("init done\n");

    uint64_t totalNs = 0; // can overflow
    void (*f)() = jobs[job];
    printf("running: %s\n", jobs_desc[job].c_str());
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

