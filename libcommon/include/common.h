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

#ifdef NDEBUG
    #define dprintf(...)
    #define eprintf(...)
#else
    #include <stdio.h>
    #include <stdlib.h>
    #define dprintf(t, args...) \
        fprintf(stderr, "[%s:%s:%d] " t "\n", __FILE__, __func__, __LINE__, ## args)
    #define eprintf(c, t, args...) do { \
            fprintf(stderr, "[%s:%s:%d] " t "\n", __FILE__, __func__, __LINE__, ## args); \
            exit(c); } while(0)
#endif /* NDEBUG */

#define fdprintf(t, args...) \
    fprintf(stderr, "[%s:%d:%s] " t "\n", __FILE__, __LINE__, __func__, ## args)

/* send switch */
#define SEND_CMPL 1
#define SEND_INLINE 1

/* run show_gids to get the info */
#define NUM_DEVICES 2
#define DEVICE_NAME "mlx5_1"
#define DEVICE_GID 3
#define PORT_NUM 1

#define CQ_NUM_DESC 64

/* size for local buffers, 512M */
#define SEND_BUF_SIZE (512 << 20)
#define RECV_BUF_SIZE (512 << 20)

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

extern uint64_t send_post_id;
extern uint64_t send_poll_id;
extern uint64_t recv_post_id;
extern uint64_t recv_poll_id;

int init(int type, const char * server_url);
int steer();

uint64_t send(void * buf, size_t size);
uint64_t send_async(void * buf, size_t size);
uint64_t send_async_sge(struct ibv_sge *sge, int num_sge);

uint64_t recv(void * buf, size_t size);
uint64_t recv_async(void * buf, size_t size);

int poll(uint64_t wr_id);
int poll_cq(struct ibv_cq *cq, int n, struct ibv_wc *wc);

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

static inline uint64_t microtime() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t t = ts.tv_sec*1000*1000 + ts.tv_nsec/1000;
    return t;
}

#ifdef __cplusplus
}
#endif

#endif
