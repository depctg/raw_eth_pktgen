#define _POSIX_C_SOURCE (199309L)
#define _GNU_SOURCE
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include "common.h"
#include "cache.hpp"

// #define PRICE_BREAK
// #define SIMP_BREAK
// #define REPLACE_BREAK

const int linesize = 4 * 1024;
const int total_size = 256 * 1024 * 1024;
const int slots = total_size / linesize;
const uint64_t c1_raddr = 0;

using C1 = DirectCache<0,c1_raddr,0,slots,linesize,0>;
using C1R = CacheReq<C1>;

static inline void cache_init() {
	memset(_rbuf, 0, RBUF_SIZE);
	uint64_t va = 0;
	for (va = 0; va < C1::Value::bytes; va += C1::Value::linesize) {
		int *i = C1R::get<int>((void *)va);
		printf("%d", *i);
	}
	printf("\n");
}

