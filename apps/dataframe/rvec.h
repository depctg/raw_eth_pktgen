#ifndef _RVECTOR_H_
#define _RVECTOR_H_

#include <cstdlib>
#include <vector>
#include <cstdint>
#include <cstring>
#include "side_channel.h"
#include "cache.h"
#include "common.h"


template <typename T>
struct rvector {
  T * head;
  T * end;
  T * tail;
};

template <typename T>
void remotelize(std::vector<T> &v) {
    rvector<T> * rv = (rvector<T> *) &v;
    size_t s = v.size();
    size_t c = v.capacity();

    // [2 ... 4k ]
    // [2 ... 4k + sizeof(v1) ]

    // replace with remotelize code
    void * raddr = _disagg_alloc(2, sizeof(T) * c);
    
    unsigned channel = channel_create(
      (uint64_t)raddr, c, sizeof(T),
      sizeof(T), 1024/sizeof(T), 1024/sizeof(T), 0, 0, 1
    );
    for (size_t i = 0; i < c; ++ i) {
      int* di = (int*) channel_access(channel, i);
      di[0] = rv->head[i];
    }
    channel_destroy(channel);

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