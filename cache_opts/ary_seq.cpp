#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "../common.h"
#include "../packet.h"
#include "../app.h"

int main(int argc, char * argv[]) {
    int msg_size = 16;
	init(TRANS_TYPE_RC, "tcp://localhost:3456");

    send(sbuf, msg_size);
}
