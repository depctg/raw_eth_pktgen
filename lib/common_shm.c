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

    // init local buf only
    if (type != TRANS_TYPE_SHM_EXECUTOR) {
        // TODO: atomic set of pointers
        // see https://en.cppreference.com/w/c/atomic/atomic_is_lock_free
        memset(ldata->ringbufs, 0, 2*sizeof(struct ringbuf));

        sbuf = (void *)ldata->sbuf;
        rbuf = (void *)ldata->rbuf;
    }
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

uint64_t send_async(void *buf, size_t size) {
    uint64_t id = 0;
    uint64_t offset = (char *)buf - (char *)ldata;
    ringbuf_commit(&ldata->ringbufs[0], offset, id, size);
    return id;
}

uint64_t send_async_sge(struct ibv_sge *sge, int num_sge) {
    perror("Not Implemeted in Local");
    return 0;
}

uint64_t send(void * buf, size_t size) {
    uint64_t id = 0;
    uint64_t offset = (char *)buf - (char *)ldata;
    ringbuf_commit(&ldata->ringbufs[0], offset, id, size);
    // TODO: better one sided execution
    struct ibv_wc wc;
    while (!poll_cq(NULL, 1, &wc)) ;
    return id;
}

uint64_t recv(void * buf, size_t size) {
    uint64_t id = ++post_id;
    uint64_t offset = (char *)buf - (char *)ldata;
    ringbuf_commit(data_rq(ldata), offset, id, size);
    // TODO: better one sided execution
    poll(id);
    return id;
}

uint64_t recv_async(void * buf, size_t size) {
    uint64_t id = ++post_id;
    uint64_t offset = (char *)buf - (char *)ldata;
    ringbuf_commit(data_rq(ldata), offset, id, size);
    return id;
}

int poll(uint64_t wr_id) {

    while (poll_id < wr_id) {
        struct ringbuf *buf;
        // Poll RQ first
        for (int i = 1; i >= 0; i--) {
            buf = &ldata->ringbufs[i];
            if (buf->ackp != buf->executep) {
                buf->ackp = buf->executep;
                unsigned lastexec = ringbuf_ptrprev(buf,executep);
                uint64_t id = buf->reqs[lastexec].wrid;
                if (id > poll_id) poll_id = id;
                // if (buf->execute_id > poll_id) poll_id = buf->execute_id;
            }
        }
    };

    return 0;
}

int poll_cq(struct ibv_cq *cq, int n, struct ibv_wc *wc) {
    struct ringbuf *buf;
    int cnt = 0;
    for (int i = 0; i < 2; i++) {
        buf = &ldata->ringbufs[i];
        for (; buf->ackp != buf->executep && cnt < n;
               buf->ackp = (buf->ackp+1) % RINGBUF_CAP) {
            wc[cnt].wr_id = buf->reqs[buf->ackp].wrid;
            wc[cnt].status = 0;
            cnt++;
        }
    }

    return cnt;
}

