#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "workload.hpp"
#include "cache.h"
#include "util.hpp"

// batch, prefetch


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
    // printf("%lu\n", arci->tail - node);
    arci->head = node + nextRand(N_node);
  }
}

// n_blocks * eles = M_arc
// CLS 4KB
// #define n_blocks 1048576
// #define eles 64

// CLS 2MB
#define n_blocks 2048
#define eles 32768

void visit() {
    // token_t prefetch_tokens[];
    uint64_t k = 0;
    for (int j = 0; j < n_blocks; j++ ) {
        // cache_request(j + 1);
        // arc_t * p = (arc_t *) cache_access_mod_opt_mut(arc + j * eles);
        cache_token_t tk_arc = cache_request((uint64_t) (arc + j * eles)); 
        arc_t *p = (arc_t *) cache_access_mut(tk_arc); 
        for( int i = 0; i < eles; i++ ) {
            arc_t *arci = p + i;
            k += 1;
            cache_token_t tk_node_tail = cache_request((uint64_t) (arci->tail));
            node_t *node_tail = (node_t *) cache_access_mut(tk_node_tail);
            arci->nextout = node_tail->firstout;
            node_tail->firstout = arc + j * eles + i;

            cache_token_t tk_node_head = cache_request((uint64_t) (arci->head));
            node_t *node_head = (node_t *) cache_access_mut(tk_node_head);
            // prefetch address
            //     (arci + 1)->head
            arci->nextin = node_head->firstin;
            node_head->firstin = arc + j * eles + i;
        }
    }
  printf("%lu\n", k);
}

void do_work() {
  setup();
  printf("After setup\n");
  uint64_t start = microtime();
  visit();
  uint64_t end = microtime();

  printf("Exec time %.5f s\n", (end - start)/1e6);
  // check();
}

int main () {
  init_client();
  cache_init();
  // channel_init();

  do_work();
  return 0;
}
