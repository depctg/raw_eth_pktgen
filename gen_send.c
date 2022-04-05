#include <string.h>
#include <stdio.h>
#include "common.h"
#include "packet.h"

struct req {
    enum {
        REQ_WRITE,
        REQ_READ,
    };
    int index;
    int elsize;
};

int main() {
	init(TRANS_TYPE_UDP);
	steer();
	// copy packet
	memcpy(sbuf, pkt_template, sizeof(pkt_template));

	// send
	// printf("begin send\n");
	// send(sbuf, sizeof(pkt_data));

	// recv
	printf("begin recv\n");
	recv(rbuf, PKT_SIZE);

	printf("We are done\n");

	return 0;

        // local
        const int size = 1024;
        int a[size];
        int sum = 0;
        for (int i = 0; i < size; i++) {
            sum += a[i];
        }

        // remote
        const int size = 1024;
        int a[size];
        int sum = 0;
        for (int i = 0; i < size; i++) {
            sum += a[i];
        }

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

        // remote3 
        reqs[0].index = 0;
        reqs[0].size = sizeof(int);
        send_aync(&req[0], sizeof(struct req));
        int id = recv_aync(&((int *)rbuf)[i], sizeof(int));

        for (int i = 0; i < size; i++) {
            int ii = (i + 1) % num_reqs;
            reqs[ii].index = 0;
            reqs[ii].size = sizeof(int);
            send_aync(&req[ii], sizeof(struct req));
            // send req
            // recv result
            poll(id);
            // get data from buffer
            sum += ((int *)rbuf)[i];
        }

        // remote4
        reqs[0].index = 0;
        reqs[0].size = sizeof(int) * 16;
        send_aync(&req[0], sizeof(struct req));
        int id = recv_aync(&((int *)rbuf)[i], sizeof(int) * 16);





}

