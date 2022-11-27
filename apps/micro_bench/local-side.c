#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "common.h"
#include "cache.h"
#include "mapp.h"
#include "helper.h"
#include "side_channel.h"

#define k 1
static inline void foo(dat_t *node) {
  // memory component
  for (int i = 0; i < MEMORY_COMPONENT; ++ i) {
    node->payload[i] = 0x1F;
  }

  static volatile int64_t j = 0;
  // Compute component
  for (int i = 1; i < COMPUTE_COMPONENT + 1; ++ i) {
    j += i;
    j *= i;
    j %= i;
  }
  (void)j;
}


int main(int argc, char* argv[]) {
  int depth = atoi(argv[1]);
  int batch = atoi(argv[2]);
  printf("prefetch depth: %d\n", depth);
  init_client();
  cache_init();
  channel_init();

  dat_t* A = _disagg_alloc(2, (256ULL << 20) / sizeof(dat_t) * sizeof(dat_t));

  uint64_t start = getCurNs();
  unsigned channel;
  channel = channel_create(
    (uintptr_t) A, INNER, sizeof(dat_t), depth+batch, batch, depth, CHANNEL_STORE
  );
  for (int j = 0; j < INNER; ++ j) {
    dat_t* LA = (dat_t*) channel_access(channel, j);
    foo(LA);
  }
  channel_destroy(channel);
  uint64_t end = getCurNs();
  printf("Exe time: %ld us\n", (end-start)/1000);
  // examine
  channel = channel_create(
    (uintptr_t) A, INNER, sizeof(dat_t), depth+batch, batch, depth, CHANNEL_LOAD
  );
  for (int j = 0; j < INNER; ++ j) {
    dat_t* LA = (dat_t*) channel_access(channel, j);
    if (LA->payload[0] != 0x1F) {
      printf("got u\n");
      exit(1);
    }
  }
  channel_destroy(channel);
  return 0;
}