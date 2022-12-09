#ifndef _RVECTOR_H_
#define _RVECTOR_H_

#include <vector>
#include <stdint.h>
#include "app.h"
#include "cache.h"
#include "side_channel.h"
#include "common.h"
#include "rring.h"

extern "C" {
void init_client();
void cache_init();
void channel_init();

void * _disagg_alloc(unsigned cache, size_t size);
void * cache_access_mut(cache_token_t token);
void * cache_access(cache_token_t token);
cache_token_t cache_request(uint64_t vaddr);
unsigned channel_create(
  // app stats
  uint64_t original_start_vaddr, 
  uint64_t upper_bound,
  uint64_t ori_unit_size, // sizeof struct that is originally accessed
  // channel settingsa
  unsigned size_each, 
  unsigned num_slots, 
  unsigned batch, 
  unsigned dist,
  // i for assembler, i+1 for dis
  uint16_t assem_id,
  // TODO: add pure store
  // load / mixed
  int kind);

void * channel_access(unsigned channel, uint64_t i);
void channel_destroy(unsigned channel);
}

template <typename T>
struct rvector {
  T * head;
  T * end;
  T * tail;
};

static inline uint64_t remoteAddr(void *p) {
  virt_addr_t vaddr = { .ser = (uint64_t)p };
  return vaddr.addr + RPC_RET_LIMIT;
}

template <typename T>
void remotelize(std::vector<T> &v) {
    rvector<T> * rv = (rvector<T> *) &v;
    size_t s = v.size();
    size_t c = v.capacity();

    // [2 ... 4k ]
    // [2 ... 4k + sizeof(v1) ]

    // replace with remotelize code
    void * raddr = _disagg_alloc(2, sizeof(T) * c);
    
    // unsigned channel = channel_create(
    //   (uint64_t)raddr, c, sizeof(T),
    //   sizeof(T), 4096/sizeof(T), 4096/sizeof(T), 0, 0, 1
    // );
    // for (size_t i = 0; i < c; ++ i) {
    //   T* di = (T*) channel_access(channel, i);
    //   di[0] = rv->head[i];
    // }
    // channel_destroy(channel);

    // All Types are pow2, so it's OK
    rring_init(writer, T, (2 << 20), 32, (size_t) ((char*)rbuf + (8 << 20)), remoteAddr(raddr));

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

    // __int128_t token = cache_request((uintptr_t) raddr);
    // void * rdata = cache_access_mut(&token);
    // memcpy(rdata, rv->head, c * sizeof(T));

    v.clear();
    v.shrink_to_fit();

    rv->head = (T *) (raddr);
    rv->end = rv->head + s;
    rv->tail = rv->head + c;
}


#endif
