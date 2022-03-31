#include <infiniband/verbs.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"
#include "packet.h"

/* global data */
void *sbuf, *rbuf;
struct ibv_qp *qp;
struct ibv_cq *cq;
struct ibv_mr *smr, *rmr;

int init(int type) {
	struct ibv_context *context = NULL;
	struct ibv_pd *pd;

	int ret;

	int num_devices = NUM_DEVICES;

	struct ibv_device** device_list = ibv_get_device_list(&num_devices);
	for (int i = 0; i < num_devices; i++){
		printf("device %i, %s | %s\n", i, ibv_get_device_name(device_list[i]), DEVICE_NAME);
		if (strcmp(DEVICE_NAME, ibv_get_device_name(device_list[i])) == 0) {
			context = ibv_open_device(device_list[i]);
			break;
		}
	}

	ibv_free_device_list(device_list);

	if (!context) {
		fprintf(stderr, "Couldn't get context for %s\n", DEVICE_NAME);
		exit(1);
	}

	/* 3. Allocate Protection Domain */
	/* Allocate a protection domain to group memory regions (MR) and rings */
	pd = ibv_alloc_pd(context);
	if (!pd) {
		fprintf(stderr, "Couldn't allocate PD\n");
		exit(1);
	}


	/* 4. Create Complition Queue (CQ) */
	cq = ibv_create_cq(context, CQ_NUM_DESC, NULL, NULL, 0);
	if (!cq) {
		fprintf(stderr, "Couldn't create CQ %d\n", errno);
		exit (1);
	}

	/* 5. Initialize QP */
	struct ibv_qp_init_attr qp_init_attr = {
		.qp_context = NULL,
		/* report send completion to cq */
		.send_cq = cq,
		.recv_cq = cq,
		.cap = {
			/* no send ring */
			.max_send_wr = CQ_NUM_DESC,
			.max_send_sge = 1,
			/* maximum number of packets in ring */
			.max_recv_wr = CQ_NUM_DESC,
			.max_recv_sge = 1,
			/* if inline maximum of payload data in the descriptors themselves */
			.max_inline_data = 512,

		},

		.qp_type = type == TRANS_TYPE_UDP ? IBV_QPT_RAW_PACKET :
                           IBV_QPT_RC;
	};


	/* 6. Create Queue Pair (QP) - Send Ring */
	qp = ibv_create_qp(pd, &qp_init_attr);
	if (!qp) {
		fprintf(stderr, "Couldn't create QP\n");
		exit(1);
	}

	/* 7. Initialize the QP (receive ring) and assign a port */
	struct ibv_qp_attr qp_attr;
	int qp_flags;
	memset(&qp_attr, 0, sizeof(qp_attr));
	qp_flags = IBV_QP_STATE | IBV_QP_PORT;
	qp_attr.qp_state = IBV_QPS_INIT;
	qp_attr.port_num = 1;

	// INIT
	ret = ibv_modify_qp(qp, &qp_attr, qp_flags);
	if (ret < 0) {
		fprintf(stderr, "failed modify qp to init\n");
		exit(1);
	}

	// INIT->RTR
	memset(&qp_attr, 0, sizeof(qp_attr));
	qp_flags = IBV_QP_STATE;
	qp_attr.qp_state = IBV_QPS_RTR;

	ret = ibv_modify_qp(qp, &qp_attr, qp_flags);
	if (ret < 0) {
		fprintf(stderr, "failed modify qp to receive\n");
		exit(1);
	}

	// RTR->RTS
	qp_flags = IBV_QP_STATE;
	qp_attr.qp_state = IBV_QPS_RTS;

	ret = ibv_modify_qp(qp, &qp_attr, qp_flags);
	if (ret < 0) {
		fprintf(stderr, "failed modify qp to receive\n");
		exit(1);
	}

	/* 9. Register MR */
	sbuf = malloc(SEND_BUF_SIZE);
	rbuf = malloc(RECV_BUF_SIZE);

	if (!sbuf || !rbuf) {
		fprintf(stderr, "Coudln't allocate memory\n");
		exit(1);
	}

	smr = ibv_reg_mr(pd, sbuf, SEND_BUF_SIZE, IBV_ACCESS_LOCAL_WRITE);
	rmr = ibv_reg_mr(pd, rbuf, RECV_BUF_SIZE, IBV_ACCESS_LOCAL_WRITE);
	if (!smr || !rmr) {
		fprintf(stderr, "Couldn't register mr\n");
		exit(1);
	}

	return 0;
}

int steer() {
	/* 13. Create steering rule for recv */
	struct raw_eth_flow_attr {
		struct ibv_flow_attr attr;
		struct ibv_flow_spec_eth spec_eth;
	} __attribute__((packed)) flow_attr = {
		.attr = {
			.comp_mask = 0,
			.type = IBV_FLOW_ATTR_NORMAL,
			.size = sizeof(flow_attr),
			.priority = 0,
			.num_of_specs = 1,
			.port = PORT_NUM,
			.flags = 0,
		},
		.spec_eth = {
			.type   = IBV_FLOW_SPEC_ETH,
			.size   = sizeof(struct ibv_flow_spec_eth),
			.val = {
				.dst_mac = { DST_MAC },
				.src_mac = { SRC_MAC },
				.ether_type = 0,
				.vlan_tag = 0,
			},
			.mask = {
				.dst_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
				.src_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
				.ether_type = 0,
				.vlan_tag = 0,
			}
		}

	};

	struct ibv_flow *eth_flow;
	eth_flow = ibv_create_flow(qp, &flow_attr.attr);

	if (!eth_flow) {
		fprintf(stderr, "Couldn't attach steering flow\n");
		exit(1);
	}
}

int steer_udp(uint16_t steer_port, uint16_t src_port) {

    uint16_t dst_prt_hi = (steer_port >> 8) & 0x00FF;
    uint16_t dst_prt_lo = (steer_port << 8) & 0xFF00;


    uint16_t src_prt_hi = (src_port >> 8) & 0x00FF;
    uint16_t src_prt_lo = (src_port << 8) & 0xFF00;

    uint16_t dst_port_big_end = dst_prt_hi | dst_prt_lo;
    uint16_t src_port_big_end = src_prt_hi | src_prt_lo;
    // printf("Steering Rule: dst port  %d, %04x\n", steer_port, dst_port_big_end);


    /* 13. Create steering rule for recv */
    struct raw_eth_flow_attr_udp {
        struct ibv_flow_attr attr;
        struct ibv_flow_spec_eth spec_eth;
        struct ibv_flow_spec_tcp_udp spec_udp;
    } __attribute__((packed)) flow_attr = {
        .attr = {
            .comp_mask = 0,
            .type = IBV_FLOW_ATTR_NORMAL,
            .size = sizeof(flow_attr),
            .priority = 0,
            .num_of_specs = 2,
            .port = PORT_NUM,
            .flags = 0,
        },
        .spec_eth = {
            .type   = IBV_FLOW_SPEC_ETH,
            .size   = sizeof(struct ibv_flow_spec_eth),
            .val = {
                .dst_mac = { LOCAL_MAC },
                .src_mac = {0},
                .ether_type = 0,
                .vlan_tag = 0,
            },
            .mask = {
                .dst_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                .src_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                .ether_type = 0,
                .vlan_tag = 0,
            }
        },
        .spec_udp = {
            .type   = IBV_FLOW_SPEC_UDP,
            .size   = sizeof(struct ibv_flow_spec_tcp_udp),
            .val = {
                .dst_port = dst_port_big_end,
                .src_port = 0
            },
            .mask = {
                .dst_port = 0xFFFF,
                .src_port = 0
            }
        }

    };

    struct ibv_flow *eth_flow;
    eth_flow = ibv_create_flow(qp, &flow_attr.attr);

    if (!eth_flow) {
        fprintf(stderr, "Couldn't attach steering flow %s\n", strerror(errno));

        exit(1);
    }
}


// Send one packet, TODO: batch
int send(void * buf, size_t size) {
	int n, ret;
	struct ibv_sge sge;
	struct ibv_send_wr wr, *bad_wr;
	struct ibv_wc wc;

	/* Send Packets */
	/* scatter/gather entry describes location and size of data to send*/
	sge.addr = (uint64_t)buf;
	sge.length = size;
	sge.lkey = smr->lkey;

	memset(&wr, 0, sizeof(wr));

	wr.num_sge = 1;
	wr.sg_list = &sge;
	wr.next = NULL;
	wr.opcode = IBV_WR_SEND;

	/* inline ? */
	wr.send_flags = IBV_SEND_INLINE;

	wr.wr_id = 0;
#if SEND_CMPL
	wr.send_flags |= IBV_SEND_SIGNALED;
#endif

	/* push descriptor to hardware */
	ret = ibv_post_send(qp, &wr, &bad_wr);
	if (ret < 0) {
		fprintf(stderr, "failed in post send\n");
		exit(1);
	}

	/* poll for completion after half ring is posted */
#if SEND_CMPL
	n = ibv_poll_cq(cq, 1, &wc);
	if (n > 0) {
		printf("completed message %ld\n", wc.wr_id);
	} else if (msgs_completed < 0) {
		printf("Polling error\n");
		return 1;
	}
#endif

	return 0;
}

int recv(void * buf, size_t size) {
	int n, ret;
	struct ibv_sge sge;
	struct ibv_recv_wr wr, *bad_wr;
	struct ibv_wc wc;

	sge.addr = (uintptr_t)buf;
	sge.length = size;
	sge.lkey = rmr->lkey;
	 
	wr.num_sge = 1;
	wr.sg_list = &sge;
	wr.next = NULL;

	ret = ibv_post_recv(qp, &wr, &bad_wr);
	if (ret < 0) {
		fprintf(stderr, "failed in post recv\n");
		exit(1);
	}

	printf("begin poll\n");
	while ((n = ibv_poll_cq(cq, 1, &wc)) == 0);
	printf("message received %d\n", n);
	return 0;
}

