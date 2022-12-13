#ifndef _RVECTOR_H_
#define _RVECTOR_H_

#include <cstdlib>
#include <vector>
#include <cstdint>
#include <cstring>
#include "side_channel.h"
#include "cache.h"
#include "common.h"
#include "rring.h"


template <typename T>
struct rvector {
  T * head;
  T * end;
  T * tail;
};

#define ring_start_base ((char*)rbuf + (8UL << 20))

static inline uint64_t remoteAddr(void *p) {
  virt_addr_t vaddr = { .ser = (uint64_t)p };
  return vaddr.addr + RPC_RET_LIMIT;
}


#define access_block_size (2ULL << 9)

template <typename T>
void remotelize(unsigned cid, std::vector<T> &v) {
#if 0
    rvector<T> * rv = (rvector<T> *) &v;
    size_t s = v.size();
    size_t c = v.capacity();

    void * raddr = _disagg_alloc(cid, sizeof(T) * c);
    
    // unsigned channel = channel_create(
    //   (uint64_t)raddr, c, sizeof(T),
    //   sizeof(T), 2048/sizeof(T), 2048/sizeof(T), 0, 0, 1
    // );

    // for (size_t i = 0; i < c; ++ i) {
    //   T* di = (T*) channel_access(channel, i);
    //   di[0] = rv->head[i];
    // }
    // channel_destroy(channel);

    rring_init(writer, T, (2 << 20), 32, (size_t) ring_start_base, remoteAddr(raddr));

    rring_outer_loop(writer, T, c) {
      rring_inner_preloop(writer, T);
      rring_sync_writeonly(writer);

      rring_inner_loop(writer, j) {
        size_t nth = _t_writer * _bn_writer + j;
        _inner_writer[j] = rv->head[nth];
      }
      rring_inner_wb(writer);
    }
    rring_cleanup_writeonly(writer);

    v.clear();
    v.shrink_to_fit();

    rv->head = (T *) (raddr);
    rv->end = rv->head + s;
    rv->tail = rv->head + c;
#endif

}

#endif