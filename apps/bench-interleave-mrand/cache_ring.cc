#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "workload.hpp"
#include "cache.h"
#include "rring_cache.h"


void setup() {
  node = (node_t *) _disagg_alloc(2, sizeof(node_t) * N_node);
  arc = (arc_t *) _disagg_alloc(2, sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    node_t *nodei = (node_t *) cache_access_mod_opt_mut(node + i);
    nodei->number = -i;
    nodei->firstin = arc + nextRand(M_arc);
    nodei->firstout = arc + nextRand(M_arc);
  }

  for (int i = 0; i < M_arc; ++ i) {
    arc_t *arci = (arc_t *) cache_access_mod_opt_mut(arc + i);
    arci->tail = node + nextRand(N_node);
    arci->head = node + nextRand(N_node);
  }
}

void visit() {
  for( int i = 0; i < M_arc; i++ )
  {
    // TODO: need checking for direct mapped 
    

    arc_t *arci = (arc_t *) cache_access_mod_opt_mut(arc + i);
    node_t *node_tail = (node_t *) cache_access_mod_opt_mut(arci->tail);

    arci->nextout = node_tail->firstout;
    node_tail->firstout = arc + i;

    arci = (arc_t *) cache_access_mod_opt_mut(arc + i);
    node_t *node_head = (node_t *) cache_access_mod_opt_mut(arci->head);

    arci->nextin = node_tail->firstin;
    node_head->firstin = arc + i;
  }
}

void check() {
  uint64_t check_sum = 0;
  for (int i = 0; i < M_arc; ++ i) {
    arc_t *arci = (arc_t *) cache_access_mod_opt(arc + i);
    node_t *node_tail = (node_t *) cache_access_mod_opt(arci->tail);
    check_sum += node_tail->number;

    arci = (arc_t *) cache_access_mod_opt(arc + i);
    node_t *node_head = (node_t *) cache_access_mod_opt(arci->head);
    check_sum += node_head->number;
  }
  printf("Checksum = %lu\n", check_sum);
}

int main () {
  init_client();
  cache_init();

  do_work();
  return 0;
}