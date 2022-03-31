#ifndef _COMMON_H_
#define _COMMON_H_

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* send switch */
#define SEND_CMPL 0

/* run show_gids to get the info */
#define NUM_DEVICES 4
#define DEVICE_NAME "mlx5_2"

#define PORT_NUM 1
#define CQ_NUM_DESC 512

#define SEND_BUF_SIZE 1024 * 4
#define RECV_BUF_SIZE 1024 * 4

enum {
    TRANS_TYPE_UDP = 0,
    TRANS_TYPE_SENDRECV,
    TRANS_TYPE_READWRITE
};

extern void *sbuf, *rbuf;
extern struct ibv_qp *qp;
extern struct ibv_cq *cq;
extern struct ibv_mr *smr, *rmr;

int init(int type);
int steer();
int send(void * buf, size_t size);
int recv(void * buf, size_t size);

// RDMA_info exchange
struct conn_info {
    int port;
    uint32_t local_id;
    uint16_t qp_number;

    int num_mr;
    union ibv_gid gid;
    struct ibv_mr mr[0];
};

static inline uint64_t getCurNs() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t t = ts.tv_sec*1000*1000*1000 + ts.tv_nsec;
    return t;
}

#ifdef __cplusplus
}
#endif

#endif
