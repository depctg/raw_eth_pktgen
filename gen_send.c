#include <string.h>
#include <stdio.h>
#include "common.h"
#include "packet.h"

#include "app.h"

int main() {
	init(TRANS_TYPE_UDP);
	steer();

/* local
	const int size = 1024;
	int a[size];
	int sum = 0;
	for (int i = 0; i < size; i++) {
	    sum += a[i];
	}
*/

/* remote: unoptimized
	struct req *reqs = (struct req *)sbuf;
	size_t num_reqs = SEND_BUF_SIZE / sizeof(struct req);
	reqs[0].index = 0;
	reqs[0].size = sizeof(int);
	send(&req[0], sizeof(struct req));

	const int size = 1024;
	int a[size];
	int sum = 0;
	for (int i = 0; i < size; i++) {
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
*/

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

	for (int i = BATCH_SIZE; i < ARRAY_SIZE; i += BATCH_SIZE) {
        // Send next request
        int buf_id_nxt = (buf_id + 1) % num_buf;
        reqs[buf_id_nxt].index = i * sizeof(int);
        reqs[buf_id_nxt].size = batch_size;
        send_async(reqs + buf_id_nxt, sizeof(struct req));
        wr_id_nxt = recv_async((int *)rbuf + buf_id_nxt * BATCH_SIZE, batch_size);

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

