#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "packet.h"

#include "app.h"

int main(int argc, char * argv[]) {
    int msg_size = 16;
    if (argc != 2) {
        printf("Usage: %s <connection-key\n", argv[0]);
        exit(1);
    }
	init(TRANS_TYPE_RC_SERVER, argv[1]);

    recv(rbuf, msg_size * 10);
    for (int i = 0; i < msg_size / 4; ++ i)
    {
        printf("%d\n", *((int *)rbuf + i));
    }
}
