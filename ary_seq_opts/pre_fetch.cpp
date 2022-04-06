#include "../common.h"
#include "../packet.h"
#include "../app.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <queue>

using namespace std;

int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);

	// strided_prefetch
    int batch_size = sizeof(int) * BATCH_SIZE;
	struct req *reqs = (struct req *)sbuf;
    uint64_t wr_id_cur;
    queue<pair<uint64_t, int>> wr_ids;

    const int num_buf = 2 * PRE_STRIDE;
    int req_step_id = 0;

    uint64_t sum = 0;
    int max_steps = ARRAY_SIZE / BATCH_SIZE;
    auto start = chrono::steady_clock::now();

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

    auto end = chrono::steady_clock::now();
    std::cout << "SUM " << sum 
              << ", ns: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
              << std::endl;
    return 0;

}


