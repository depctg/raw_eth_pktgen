#ifndef _RINGBUF_H_
#define _RINGBUF_H_
#include <stdatomic.h>
#include "common.h"

#define RINGBUF_CAP (1024)

struct ringbuf {
    atomic_uint commitp, executep;
    uint64_t addr[RINGBUF_CAP];
    uint64_t wrid[RINGBUF_CAP];
    size_t size[RINGBUF_CAP];
};

// TODO: amotic
#define ringbuf_get(buf,p,v) ((buf)->v[(buf)->p])

static inline int ringbuf_avaliable(struct ringbuf *buf) {
    return (buf->commitp != buf->executep);
}

static inline void ringbuf_commit(struct ringbuf * buf,
        uint64_t addr, uint64_t wrid, size_t size) { 
    unsigned next = (buf->commitp + 1) % RINGBUF_CAP;
    // spin wait on the ringbuf
    // while (unlikely(next == buf->execp)) ;
    unsigned idx = buf->commitp;
    atomic_store(&buf->commitp, next);

    buf->addr[idx] = addr;
    buf->wrid[idx] = wrid;
    buf->size[idx] = size;
}

static inline void ringbuf_execute(struct ringbuf * buf, unsigned n) { 
    // if (unlikely(buf->execp == buf->commitp)) return;
    unsigned cur,next;
    do {
        cur = buf->executep;
        next = (cur + n) % RINGBUF_CAP;
    } while (!atomic_compare_exchange_weak(&buf->executep, &cur, next));
}

/* shared data for shm
 * TODO: move this out */
struct shared_data {
    char sbuf[SEND_BUF_SIZE];
    char rbuf[RECV_BUF_SIZE];
    struct ringbuf ringbufs [2];
};

static inline struct ringbuf * data_sq(struct shared_data *d) {
    return &d->ringbufs[0];
}
static inline struct ringbuf * data_rq(struct shared_data *d) {
    return &d->ringbufs[1];
}

static const size_t aligned_datasize = sizeof(struct shared_data);
static const size_t shmsize = aligned_datasize * 2;

/* transfer functions */
void execute_transfer(struct shared_data *x, struct shared_data *y);

#endif
