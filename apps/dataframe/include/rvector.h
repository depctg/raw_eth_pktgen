#ifndef _RVECTOR_H_
#define _RVECTOR_H_

#include <vector>
#include <stdint.h>
#include "app.h"
#include "common.h"
#include "rring_cache.h"

// #define NIDX 107957636
#define NIDX (1024 * 1024 * 128)
#define fullsize (30ULL >> 30)
#define cache_ratio (0.9)

#define rvid_ofst       (1024ULL * 1024 * 2)
#define rpickdate_ofst  (rvid_ofst + sizeof(int) * NIDX) 
#define rdropdate_ofst  (rpickdate_ofst + sizeof(SimpleTime) * NIDX)
#define rpsgcnt_ofst    (rdropdate_ofst + sizeof(SimpleTime) * NIDX) 
#define rtripdist_ofst  (rpsgcnt_ofst + sizeof(int) * NIDX)   
#define rplon_ofst      (rtripdist_ofst + sizeof(double) * NIDX)
#define rplat_ofst      (rplon_ofst + sizeof(double) * NIDX)
#define rrateid_ofst    (rplat_ofst + sizeof(double) * NIDX)
#define rflag_ofst      (rrateid_ofst + sizeof(int) * NIDX)
#define rdlon_ofst      (rflag_ofst + sizeof(char) * NIDX)
#define rdlat_ofst      (rdlon_ofst + sizeof(double) * NIDX)
#define rptype_ofst     (rdlat_ofst + sizeof(double) * NIDX)
#define rfare_ofst      (rptype_ofst + sizeof(int) * NIDX)
#define rextra_ofst     (rfare_ofst + sizeof(double) * NIDX)
#define rmta_ofst       (rextra_ofst + sizeof(double) * NIDX)
#define rtip_ofst       (rmta_ofst + sizeof(double) * NIDX)
#define rtolls_ofst     (rtip_ofst + sizeof(double) * NIDX)
#define rimpv_ofst      (rtolls_ofst + sizeof(double) * NIDX)
#define rtotal_ofst     (rimpv_ofst + sizeof(double) * NIDX)
#define rids_ofst       (rtotal_ofst + sizeof(double) * NIDX)
#define rhaversine_ofst (rids_ofst + sizeof(size_t) * NIDX)

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
