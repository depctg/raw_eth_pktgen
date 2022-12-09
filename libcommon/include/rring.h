#ifndef _RRING_H_
#define _RRING_H_

#include "common.h"

static struct ibv_wc wc[16];

#define rring_init(rn,T,bsize,nblocks,lbase,rbase) \
  const size_t _bsize_##rn = bsize, _bn_##rn = bsize / sizeof(T); \
  const size_t _nblocks_##rn = nblocks; \
  size_t _h_##rn = 0, _t_##rn = 0, _r_##rn = 0; \
  const uint64_t _lbase_##rn = (uint64_t)(lbase), _rbase_##rn = (uint64_t)(rbase)

#define rring_outer_loop(rn,T,lim) \
  for (size_t _lim_##rn = (lim), _tlim_##rn = ((lim) + _bn_##rn - 1) / _bn_##rn; \
          _t_##rn < _tlim_##rn; _t_##rn ++)
#define rring_outer_loop_with(rn,lim) \
  size_t _lim_##rn = (lim), _tlim_##rn = ((lim) + _bn_##rn - 1) / _bn_##rn
#define rring_outer_loop_with_post(rn) \
  _t_##rn ++

#define rring_inner_preloop(rn,T) \
  size_t _ilim_##rn = _lim_##rn > (_t_##rn + 1) * _bn_##rn ? \
                      _bn_##rn : _lim_##rn - _t_##rn * _bn_##rn; \
  T * _inner_##rn = (T *)(_lbase_##rn + (_t_##rn % _nblocks_##rn) * _bsize_##rn)

#define rring_sync(rn) \
  rring_poll(&_r_##rn, _t_##rn)

#define rring_sync_writeonly(rn) \
  rring_poll_only()

#define rring_inner_loop(rn, j) \
  for (size_t j = 0; j < _ilim_##rn ; j++)

#define rring_inner_wb(rn) \
    rdma(_lbase_##rn + (_t_##rn % _nblocks_##rn) * _bsize_##rn, _bsize_##rn, \
            _rbase_##rn + (_t_##rn * _bsize_##rn), 0, IBV_WR_RDMA_WRITE)

#define rring_prefetch(rn,nprefetch) \
    for (;_h_##rn < _t_##rn + nprefetch && _h_##rn < _tlim_##rn; _h_##rn ++) \
        rdma(_lbase_##rn + (_h_##rn % _nblocks_##rn) * _bsize_##rn, _bsize_##rn, \
                _rbase_##rn + (_h_##rn * _bsize_##rn), _h_##rn, IBV_WR_RDMA_READ);

static inline void rring_poll(size_t *r, size_t t) {
    if (*r >= t)
        return;

    do {
        int n = ibv_poll_cq(cq, 16, wc);
        for (int i = 0; i < n; i++) {
            if (wc[i].status != 0) {
                printf("ERROR %d, %ld\n", wc[i].status, wc[i].wr_id);
            }
            if (wc[i].wr_id > *r)
                *r = wc[i].wr_id;
        }
    } while (*r < t);
}

static inline void rring_poll_only() {
    int ret;
    do {
        ret = ibv_poll_cq(cq, 16, wc);
    } while (ret);
}

#endif
