#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "common.h"
#include "cache.h"

typedef struct A {
  int x;
  int y;
} A;

A *as;
cache_t _cache_ids[1];

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage cache_array [n]");
  }

  init_client();
  cache_init();

  _cache_ids[0] = cache_create(64, 16);
  int n = atoi(argv[1]);
  printf("%d\n", n);
  as = (A *) _disagg_alloc(_cache_ids[0], sizeof(A) * n);

  for (int i = 0; i < n; i++) {
    cache_token_t token = cache_request((intptr_t) (as + i));
    A *ai = (A *) cache_access_mut(&token);
    // printf("%p\n", ai);
    ai->x = i;
    ai->y = i * i;
  }

  for (int i = 0; i < n; i++) {
    cache_token_t token = cache_request((intptr_t) (as + i));
    A *ai = (A *) cache_access(&token);
    printf("%d: %d = %d * %d\n",i, ai->y, ai->x, ai->x);
  }

	return 0;
}