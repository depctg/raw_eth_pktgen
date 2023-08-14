#ifndef _RVECTOR_H_
#define _RVECTOR_H_

#include <vector>
#include <stdint.h>
#include "common.h"

template <typename T>
struct rvector {
  T * head;
  T * end;
  T * tail;
};

template<typename T>
rvector<T> createFromBase(uint64_t addr, size_t size, size_t cap) {
  rvector<T> rv;
  rv.head = (T *) addr;
  rv.end = rv.head + size;
  rv.tail = rv.head + cap;
  return rv;
}

template <typename T>
void remotelize(std::vector<T> &v, bool write = true) {
#if 0
    rvector<T> * rv = (rvector<T> *) &v;
    size_t s = v.size();
    size_t c = v.capacity();

    // [2 ... 4k ]
    // [2 ... 4k + sizeof(v1) ]

    // replace with remotelize code
    void * raddr = _disagg_alloc(2, sizeof(T) * c);
    
    // All Types are pow2, so it's OK
    if (write) {
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
    }

    v.clear();
    v.shrink_to_fit();

    rv->head = (T *) (raddr);
    rv->end = rv->head + s;
    rv->tail = rv->head + c;
#endif
}

template <typename T, typename C, typename CR>
void new_remotelize(std::vector<T> &v, bool write = true) {
  rvector<T> * rv = (rvector<T> *) &v;
  size_t s = v.size();
  size_t c = v.capacity();
  T *vaddr = (T*) CR::alloc(sizeof(T) * c);
  if (write) {
    uint64_t n_blocks = C::Value::bytes / C::Value::linesize;
    int eles = C::Value::linesize / sizeof(T);
    for (uint64_t j = 0; j < n_blocks; ++ j) {
      T *base = CR::template get_mut<T>(vaddr + j * eles);
      for (int i = 0; i < eles; ++ i) {
        base[i] = rv->head[j * eles + i];
      }
    }
  }
  v.clear();
  v.shrink_to_fit();

  rv->head = vaddr;
  rv->end = rv->head + s;
  rv->tail = rv->head + c;
}


template<typename T>
void dummy_reader(std::vector<T> &local_copy, uint64_t BS, unsigned N_blocks, unsigned prefetch, uint64_t s, uint64_t sbuf_offset) {
#if 0
  local_copy.reserve(s);

  rring_init(reader, T, BS, N_blocks, (size_t) ((char*)rbuf + (8<<20)), sbuf_offset);
  rring_outer_loop(reader, T, s) {

    rring_prefetch(reader, prefetch);
    rring_inner_preloop(reader, T);
    rring_sync(reader);

    rring_inner_loop(reader, j) {
      local_copy.push_back(_inner_reader[j]);
    }
  }
#endif

}

#endif
