#include <infiniband/verbs.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include "common.h"
#include "packet.h"
#include "ringbuf.h"

/* global data */
void *sbuf, *rbuf;
struct ibv_qp *qp;
struct ibv_cq *cq;
struct ibv_mr *smr, *rmr;
struct ibv_context *context = NULL;

uint64_t post_id = 0;
uint64_t poll_id = 0;

struct shared_data *ldata, *rdata;

int init(int type, const char * key_str) {

    int shmid;
	char *shmaddr;

    int shmkey = strtol(key_str, NULL, 10);
    if (shmkey == 0) {
        perror("Key is 0 or key error");
		exit(1);
    }

	shmid = shmget(shmkey, shmsize,
            // SHM_HUGETLB | SHM_HUGE_1GB | IPC_CREAT | SHM_R | SHM_W);
            SHM_HUGETLB | IPC_CREAT | SHM_R | SHM_W);
	if (shmid < 0) {
		perror("shmget");
		exit(1);
	}

    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1) {
        perror("Shared memory attach failure");
        shmctl(shmid, IPC_RMID, NULL);
        exit(2);
    }

    // assign pointers, compatable with rc
    if (type == TRANS_TYPE_SHM || type == TRANS_TYPE_RC) {
        ldata = (struct shared_data *)shmaddr;
        rdata = (struct shared_data *)shmaddr + 1;
    } else if (type == TRANS_TYPE_SHM_SERVER || type == TRANS_TYPE_RC_SERVER) {
        rdata = (struct shared_data *)shmaddr;
        ldata = (struct shared_data *)shmaddr + 1;
    } else if (type == TRANS_TYPE_SHM_EXECUTOR) {
        // same as client setup
        ldata = (struct shared_data *)shmaddr;
        rdata = (struct shared_data *)shmaddr + 1;
    }

    memset(ldata->ringbufs, 0, 2*sizeof(struct ringbuf));
    return 0;
}

int steer() {
    perror("Not Implemeted in shm");
    return -1;
}

int steer_udp(uint16_t steer_port, uint16_t src_port) {
    perror("Not Implemeted in shm");
    return -1;
}

static inline int _execute_directed(struct ringbuf *s, struct ringbuf *r) {
    unsigned send_idx = s->commitp;
    unsigned recv_idx = r->commitp;
    if (unlikely(r->size[recv_idx] < s->size[send_idx]))
        return -1;
    memcpy((void *)r->addr[recv_idx], (void *)s->addr[send_idx], s->size[send_idx]);
    ringbuf_execute(s, 1);
    ringbuf_execute(r, 1);
    return 0;
}

void execute_transfer(struct shared_data *x, struct shared_data *y) {
    // TODO: error handling
    while (1) {
        if (ringbuf_avaliable(data_sq(x)) && ringbuf_avaliable(data_rq(y)))
            _execute_directed(data_sq(x), data_rq(y));
        if (ringbuf_avaliable(data_sq(y)) && ringbuf_avaliable(data_rq(x)))
            _execute_directed(data_sq(y), data_rq(x));
    }
}

uint64_t send_async(void *buf, size_t size) {
    uint64_t id = post_id ++;
    ringbuf_commit(&ldata->ringbufs[0], (uint64_t)buf, id, size);
    return id;
}

uint64_t send_async_sge(struct ibv_sge *sge, int num_sge) {
    perror("Not Implemeted in Local");
    return 0;
}

uint64_t send(void * buf, size_t size) {
    uint64_t id = post_id ++;
    ringbuf_commit(&ldata->ringbufs[0], (uint64_t)buf, id, size);
    // TODO: better one sided execution
    poll(id);
    return id;
}

uint64_t recv(void * buf, size_t size) {
    uint64_t id = post_id ++;
    ringbuf_commit(&ldata->ringbufs[1], (uint64_t)buf, id, size);
    return id;
}

uint64_t recv_async(void * buf, size_t size) {
    uint64_t id = post_id ++;
    ringbuf_commit(&ldata->ringbufs[1], (uint64_t)buf, id, size);
    // TODO: better one sided execution
    poll(id);
    return id;
}

int poll(uint64_t wr_id) {

    do {
        if (poll_id >= wr_id)
            return 0;

        unsigned send_idx = ringbuf_get(data_sq(ldata), executep, wrid);
        if (send_idx > poll_id) poll_id = send_idx;
        unsigned recv_idx = ringbuf_get(data_rq(rdata), executep, wrid);
        if (send_idx > poll_id) poll_id = recv_idx;
    } while (1);

    return 0;
}

