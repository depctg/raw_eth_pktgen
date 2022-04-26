#include "common.h"
#include "packet.h"
#include "app.h"
#include <chrono>
#include <iostream>
#include <string.h>

int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);

	// remote_optmized
    int batch_size = sizeof(int) * BATCH_SIZE;
	struct req *reqs = (struct req *)sbuf;
    uint64_t wr_id, wr_id_nxt;

    const int num_buf = 4;
    int buf_id = 0;

    uint64_t sum = 0;
    uint64_t startNs = getCurNs();
 
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

    uint64_t endNs = getCurNs();

    printf("SUM %ld, ns: %ld\n", sum, endNs - startNs);

}

