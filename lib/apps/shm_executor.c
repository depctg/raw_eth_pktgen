#include <stdio.h>
#include <pthread.h>

#include "common.h"
#include "ringbuf.h"

static inline int _execute_directed(
        struct ringbuf *s, struct shared_data * sbase,
        struct ringbuf *r, struct shared_data * rbase) {
    unsigned send_idx = s->executep;
    unsigned recv_idx = r->executep;
    /*
    printf("execute p: %d %d, %ul <- %ul id: %d %d\n",
            send_idx, recv_idx,
            r->addr[recv_idx], s->addr[send_idx], 
            s->wrid[send_idx], r->wrid[recv_idx]
          );

    if (unlikely(r->size[recv_idx] < s->size[send_idx]))
        return -1;
    */
    memcpy(
        (char *)rbase + r->reqs[recv_idx].addr,
        (char *)sbase + s->reqs[send_idx].addr,
        s->reqs[send_idx].size);
    ringbuf_execute(s, 1);
    ringbuf_execute(r, 1);
    return 0;
}

void * execute_qp(struct shared_data **data) {
    struct shared_data *sd = data[0];
    struct shared_data *rd = data[1];
    while (1) {
        if (ringbuf_avaliable(data_sq(sd)) && ringbuf_avaliable(data_rq(rd)))
            _execute_directed(data_sq(sd), sd, data_rq(rd), rd);
    }
    return NULL;
}

void execute_transfer() {
    // TODO: error handling
    while (1) {
        /*
        printf("check SEND %d/%d/%d %d/%d/%d RECV %d/%d/%d %d/%d/%d\n",
                data_sq(ldata)->commitp, data_sq(ldata)->executep, data_sq(ldata)->ackp,
                data_rq(ldata)->commitp, data_rq(ldata)->executep, data_rq(ldata)->ackp,
                data_sq(rdata)->commitp, data_sq(rdata)->executep, data_sq(rdata)->ackp,
                data_rq(rdata)->commitp, data_rq(rdata)->executep, data_rq(rdata)->ackp);
        */
        if (ringbuf_avaliable(data_sq(ldata)) && ringbuf_avaliable(data_rq(rdata)))
            _execute_directed(data_sq(ldata), ldata, data_rq(rdata), rdata);
        if (ringbuf_avaliable(data_sq(rdata)) && ringbuf_avaliable(data_rq(ldata)))
            _execute_directed(data_sq(rdata), rdata, data_rq(ldata), ldata);
    }
}

int main(int argc, char **argv) {
    pthread_t e1, e2;
    init(TRANS_TYPE_SHM_EXECUTOR, argv[1]);

    struct shared_data *e1d[2] = {ldata, rdata};
    pthread_create(&e1, NULL, (void * (*)(void *))execute_qp, (void *)e1d);

    struct shared_data *e2d[2] = {rdata, ldata};
    pthread_create(&e2, NULL, (void * (*)(void *))execute_qp, (void *)e2d);

    pthread_join(e1, NULL);
    pthread_join(e2, NULL); 

    // execute_transfer();

    return 0;
}
