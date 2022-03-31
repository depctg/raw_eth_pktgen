#include <string.h>
#include <stdio.h>
#include "common.h"
#include "packet.h"

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
}

