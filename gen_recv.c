#include <stdio.h>

#include "common.h"
#include "packet.h"

int main() {
	init();
	steer();

	// recv
	printf("begin recv\n");
	recv(rbuf, PKT_SIZE);
	printf("We are done\n");

	return 0;
}

