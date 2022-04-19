#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include "packet.h"

#include "app.h"

int main(int argc, char * argv[]) {
    int msg_size = 16;
	init(TRANS_TYPE_RC_SERVER, "tcp://*:3456");

    const unsigned int max_recvs = 64;
    const unsigned int inflights = max_recvs / 2;
	struct ibv_wc wc[max_recvs];

    unsigned int post_recvs = 0, poll_recvs = 0;

    for (int i = 0; i < inflights; i++)
        recv_async(rbuf + i * msg_size, msg_size);
    post_recvs += inflights;

	while (1) {
        int n = ibv_poll_cq(cq, inflights, wc);

        // process the requests
        for (int i = 0; i < n; i++) {
            // not a timeout
            if (wc[i].status == 0 && wc[i].wr_id != 0) {
                int idx = (poll_recvs++) % max_recvs;

                // process request
                // sleep here to change the latency
                send_async(sbuf + idx * msg_size, msg_size);
            }
        }

        // fill the recv queue
        while (post_recvs < poll_recvs + inflights) {
            int idx = post_recvs % max_recvs;
            recv_async(rbuf + idx * msg_size, msg_size);
            post_recvs ++;
        }
	}

    return 0;

}