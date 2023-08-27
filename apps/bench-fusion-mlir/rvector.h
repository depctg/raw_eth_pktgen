#ifndef _RVECTOR_H_
#define _RVECTOR_H_

#include <vector>
#include <stdint.h>
#include <cstdio>

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

#endif
