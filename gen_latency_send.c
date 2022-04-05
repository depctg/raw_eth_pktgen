#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include "packet.h"

#include "app.h"

int main(int argc, char * argv[]) {
    int msg_size = 16;
	init(TRANS_TYPE_RC, "tcp://localhost:3456");

    // first round
    for (int i = 0; i < ARRAY_SIZE; i++) {
        send_async(sbuf, msg_size);
        uint64_t id = recv_async(rbuf, msg_size);
        poll(id);
    }

    printf("start testing..\n");

    uint64_t s = getCurNs();
    for (int i = 0; i < ARRAY_SIZE; i++) {
        send_async(sbuf, msg_size);
        uint64_t id = recv_async(rbuf, msg_size);
        poll(id);
    }
    uint64_t e = getCurNs();
    printf("latency: %ld ns\n", (e - s)/ARRAY_SIZE);
}
