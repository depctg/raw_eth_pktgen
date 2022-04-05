#include <stdio.h>

#include "common.h"
#include "packet.h"

int main() {
	init(TRANS_TYPE_UDP);
	steer();

	// recv
	printf("begin recv\n");
	recv(rbuf, PKT_SIZE);
	printf("We are done\n");

	return 0;

        // remote1
        const int size = 1024;
        int a[size];
        int sum = 0;
        struct req *reqs = (struct req *)rbuf;
        size_t num_reqs = RECV_BUF_SIZE / sizeof(struct req);
        int * reps = (int *)sbuf;
        while (1) {
            recv(&reqs[0], sizeof(struct req));
            // get datra
            // Data copy!
            reps[0] = a[reqs[0].index];
            // sleep
            // usleep();
            send(&reps[0], sizeof(int));
        }

        // remote 2
        const int size = 1024;
        int a[size];
        int sum = 0;
        struct req *reqs = (struct req *)rbuf;
        size_t num_reqs = RECV_BUF_SIZE / sizeof(struct req);
        int * a = (int *)sbuf;
        // init a
        for (int i = 0; i < size; i++)
                a[i] = 0;
        while (1) {
            recv(&reqs[0], sizeof(struct req));
            // get datra
            // Data copy!
            // sleep
            // usleep();
            send(&a[req[0]->index], sizeof(int));
        }


}

