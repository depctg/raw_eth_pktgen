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
  printf("prefetch depth: %d\n", depth);
  init_client();
  cache_init();
  channel_init();
  
  dat_t* A = _disagg_alloc(2, (256ULL << 20) / sizeof(dat_t) * sizeof(dat_t));

  // TODO: prelogue
  // for (int i = 0; i < depth; ++ i) {
  //   cache_token_t token = cache_request((intptr_t) (A+i));
  // }
  // for (int d = 0; d < depth; ++ d) {
  //   cache_token_t token = cache_request((intptr_t) (A+k*d));
  //   (void) cache_access_nrtc(&token);
  // }

  uint64_t start = getCurNs();
  for (int j = 0; j < INNER; ++ j) {
    if (j + depth < INNER)
      (void) cache_request((intptr_t)(A+k*(j+depth)));
    cache_token_t token = cache_request((intptr_t) (A+k*j));
    dat_t* LA = (dat_t*) cache_access_nrtc_mut(&token);
    foo(LA);
  }
  uint64_t end = getCurNs();
  printf("Exe time: %ld us\n", (end-start)/1000);
  // examine
  for (int j = 0; j < INNER; ++ j) {
    cache_token_t token = cache_request((intptr_t) (A+k*j));
    dat_t* LA = (dat_t*) cache_access_nrtc(&token);
    if (LA->payload[0] != 0x1F) {
      printf("got u\n");
      exit(1);
    }
  }  
  return 0;
}