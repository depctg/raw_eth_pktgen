#ifndef _COMMON_H_
#define _COMMON_H_

#include <infiniband/verbs.h>

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* send switch */
#define SEND_CMPL 1
#define SEND_INLINE 1

/* run show_gids to get the info */
#define NUM_DEVICES 2
#define DEVICE_NAME "mlx5_0"
#define DEVICE_GID 3
#define PORT_NUM 1

#define CQ_NUM_DESC 64

/* size for local buffers, 512M */
#define SEND_BUF_SIZE 1024 * 1024 * 512
#define RECV_BUF_SIZE 1024 * 1024 * 512

#define MAX_POLL 64

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

enum {
    TRANS_TYPE_UDP = 0,
    TRANS_TYPE_RC,
    TRANS_TYPE_RC_SERVER,
    TRANS_TYPE_SHM,
    TRANS_TYPE_SHM_SERVER,
    TRANS_TYPE_SHM_EXECUTOR,
};

extern void *sbuf, *rbuf;
extern struct ibv_qp *qp;
extern struct ibv_cq *cq;
extern struct ibv_mr *smr, *rmr;
extern struct ibv_context *context;

extern uint64_t post_id;
extern uint64_t poll_id;

int init(int type, const char * server_url);
int steer();

uint64_t send(void * buf, size_t size);
uint64_t send_async(void * buf, size_t size);
uint64_t send_async_sge(struct ibv_sge *sge, int num_sge);

uint64_t recv(void * buf, size_t size);
uint64_t recv_async(void * buf, size_t size);

int poll(uint64_t wr_id);

// RDMA_info exchange
struct conn_info {
    union ibv_gid gid;
    int port;
    uint32_t local_id;
    uint16_t qp_number;

    int num_mr;
    struct ibv_mr mr[0];
};
struct conn_info * server_exchange_info(const char * server_url);
struct conn_info * client_exchange_info(const char * server_url);

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
