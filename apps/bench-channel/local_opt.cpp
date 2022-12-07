#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "cache.h"
#include "helper.h"
#include "side_channel.h"
#include "workload.hpp"


void setup() {
  unsigned channel = channel_create(
    (uintptr_t) dat, M, sizeof(arc_t), 
    sizeof(arc_t), 32, 32, 0, 0, CHANNEL_STORE
  );
  for (int i = 0; i < M; ++ i) {
    // cache_token_t token = cache_request((uintptr_t)(dat + i));
    // arc_t* dat_i = (arc_t*) cache_access_mut(&token);
    arc_t* dat_i = (arc_t*) channel_access(channel, i);

    int nexti = nextRand();
    if (nexti == i)
      nexti = (nexti + 1) % M;
    dat_i->next = dat + nexti;

    int previ = nextRand();
    if (previ == i)
      previ = (previ + 1) % M;
    dat_i->prev = dat + previ;  

    dat_i->i = i;
    dat_i->j = i * i;
    dat_i->hit = 1;
    dat_i->x = i;
    dat_i->y = -1 * i;
  }
  channel_destroy(channel);
}

void obv_work(arc_t *arc, unsigned n) {
  while (n) {
    cache_token_t token = cache_request((uintptr_t)(arc));
    arc_t* arc_local = (arc_t*) cache_access_mut(token);
    dprintf("%ld: %lu %lu %u", arc-dat, arc_local->i, arc_local->j, arc_local->hit);

    arc_local->i *= n;
    arc_local->j -= n;
    arc_local->hit ++;
    arc = arc_local->next;
    n--;
  }
}

void seq_work(arc_t *arc, unsigned n) {
  unsigned channel = channel_create(
    (uintptr_t) arc, n, sizeof(arc_t), 
    sizeof(arc_t), 32, 32, 0, 0, CHANNEL_STORE
  );
  for (unsigned i = 0; i < n; ++ i) {
    // cache_token_t token = cache_request((uintptr_t)(arc+i));
    // arc_t* arc_i = (arc_t*) cache_access_mut(&token);
    arc_t* arc_i = (arc_t*) channel_access(channel, i);
    dprintf("%d: %lu %lu %u %d %d", i, arc_i->i, 
                                       arc_i->j, 
                                       arc_i->hit, 
                                       arc_i->x, 
                                       arc_i->y);

    arc_i->x *= arc_i->hit | i;
    arc_i->y *= arc_i->hit ^ i;
    arc_i->hit ++ ;
  }
  channel_destroy(channel);
}

void check() {
  unsigned channel = channel_create(
    (uintptr_t) dat, M, sizeof(arc_t), 
    sizeof(arc_t), 64, 32, 32, 0, CHANNEL_LOAD
  );
  for (int i = 0; i < M; ++ i) {
    // printf("%d - %d\n", i, M);
    // cache_token_t token = cache_request((uintptr_t)(dat + i));
    // arc_t* dat_i = (arc_t*) cache_access(&token);
    arc_t* dat_i = (arc_t*) channel_access(channel,i);

    dprintf("%d: %lu %lu %u %d %d", i, dat_i->i, 
                                       dat_i->j, 
                                       dat_i->hit, 
                                       dat_i->x, 
                                       dat_i->y);

    checksum += dat_i->x + dat_i->y;
    checksum += dat_i->i + dat_i->j;
    checksum += dat_i->hit;
  }
  channel_destroy(channel);
  printf("checksum: %lu\n", checksum);
}

int main(int argc, char **argv) {
  init_client();
  cache_init();
  channel_init();

  dat = (arc_t *) _disagg_alloc(2, sizeof(arc_t) * M);
  
  do_work();

  get_cache_logs();
  check();
}
