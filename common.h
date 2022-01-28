#ifndef _COMMON_H_
#define _COMMON_H_

#include <stddef.h>

/* send switch */
#define SEND_CMPL 0

/* run show_gids to get the info */
#define NUM_DEVICES 4
#define DEVICE_NAME "mlx5_2"

#define PORT_NUM 1
#define CQ_NUM_DESC 512

#define SEND_BUF_SIZE 1024 * 4
#define RECV_BUF_SIZE 1024 * 4

extern void *sbuf, *rbuf;
extern struct ibv_qp *qp;
extern struct ibv_cq *cq;
extern struct ibv_mr *smr, *rmr;

int init();
int steer();
int send(void * buf, size_t size);
int recv(void * buf, size_t size);

#endif
