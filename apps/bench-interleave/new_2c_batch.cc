#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "workload.hpp"
#include "cache.hpp"
#include "util.hpp"

// node
const uint64_t c1_line_size = (512ULL);
const uint64_t c1_raddr = 0;
const uint64_t c1_size = (1UL << 30);
const int c1_slots = c1_size / c1_line_size;

// arc
const uint64_t c2_line_size = (2UL << 20);
const uint64_t c2_raddr = 1024UL * 1024 * 1024;
const uint64_t c2_size = (3072UL << 20);
const int c2_slots = c2_size / c2_line_size;

// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
using C1 = DirectCache<0,c1_raddr,0,c1_slots,c1_line_size,0>;
using C2 = DirectCache<c1_slots,c2_raddr,C1::Value::bytes,c2_slots,c2_line_size,1>;

using C1R = CacheReq<C1>;
using C2R = CacheReq<C2>;

void setup() {
  node = (node_t *) C1R::alloc(sizeof(node_t) * N_node);
  arc = (arc_t *) C2R::alloc(sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    node_t *nodei = C1R::get_mut<node_t>(node + i);
    nodei->number = -i;
    nodei->firstin = arc + nextRand(M_arc);
    nodei->firstout = arc + nextRand(M_arc);
  }

  for (int i = 0; i < M_arc; ++ i) {
    arc_t *arci =  C2R::get_mut<arc_t>(arc + i);
    arci->tail = node + nextRand(N_node);
    arci->head = node + nextRand(N_node);
  }
}

// CLS 2MB
const uint64_t eles = c2_line_size / sizeof(arc_t);
const uint64_t n_blocks = M_arc / eles;

// TODO: node_t and arc_t
void visit() {
  for (int j = 0; j < n_blocks; j++ ) {
    arc_t *p = C2R::get_mut<arc_t>(arc + j * eles);
    for( int i = 0; i < eles; i++ ) {
        arc_t *arci = p + i;
        node_t *node_tail = C1R::get_mut<node_t>(arci->tail);
        arci->nextout = node_tail->firstout;
        node_tail->firstout = arc + j * eles + i;

        node_t *node_head = C1R::get_mut<node_t>(arci->head);
        arci->nextin = node_head->firstin;
        node_head->firstin = arc + j * eles + i;
    }
  }
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

  do_work();
  return 0;
}
