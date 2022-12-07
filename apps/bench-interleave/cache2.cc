#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "workload.hpp"
#include "cache.h"
#include "side_channel.h"


void setup() {
  node = (node_t *) _disagg_alloc(2, sizeof(node_t) * N_node);
  arc = (arc_t *) _disagg_alloc(3, sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    cache_token_t tk_node = cache_request((uint64_t) (node + i));
    node_t *nodei = (node_t *) cache_access_mut(tk_node);
    nodei->number = -i;
    nodei->firstin = arc + nextRand(M_arc);
    nodei->firstout = arc + nextRand(M_arc);
  }

  for (int i = 0; i < M_arc; ++ i) {
    cache_token_t tk_arc = cache_request((uint64_t) (arc + i));
    arc_t *arci = (arc_t *) cache_access_mut(tk_arc);
    arci->tail = node + nextRand(N_node);
    arci->head = node + nextRand(N_node);
  }
}

void visit() {
  for( int i = 0; i < M_arc; i++ )
  {
    cache_token_t tk_arc = cache_request((uint64_t) (arc + i)); 
    arc_t *arci = (arc_t *) cache_access_mut(tk_arc);

    cache_token_t tk_node_tail = cache_request((uint64_t) (arci->tail));
    node_t *node_tail = (node_t *) cache_access_mut(tk_node_tail);

    cache_token_t tk_node_head = cache_request((uint64_t) (arci->head));
    node_t *node_head = (node_t *) cache_access_mut(tk_node_head);

    arci->nextout = node_tail->firstout;
    node_tail->firstout = arc + i;
    arci->nextin = node_tail->firstin;
    node_head->firstin = arc + i;
  }
}

void check() {
  uint64_t check_sum = 0;
  for (int i = 0; i < M_arc; ++ i) {
    cache_token_t tk_arc = cache_request((uint64_t) (arc + i)); 
    arc_t *arci = (arc_t *) cache_access_mut(tk_arc);

    cache_token_t tk_node_tail = cache_request((uint64_t) (arci->tail));
    node_t *node_tail = (node_t *) cache_access_mut(tk_node_tail);

    cache_token_t tk_node_head = cache_request((uint64_t) (arci->head));
    node_t *node_head = (node_t *) cache_access_mut(tk_node_head);

    check_sum += node_tail->number;
    check_sum += node_head->number;
  }
  printf("Checksum = %lu\n", check_sum);
}

int main () {
  init_client();
  cache_init();
  channel_init();

  do_work();
  return 0;
}