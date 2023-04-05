#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <algorithm>    // std::shuffle

#include "common.h"
#include "workload.hpp"
#include "cache.hpp"
#include "util.hpp"

// node
const uint64_t c1_line_size = (128);
const uint64_t c1_raddr = 0;
const uint64_t c1_size = (510 << 20);
const int c1_slots = c1_size / c1_line_size;

// arc
const uint64_t c2_line_size = (4 << 10);
const uint64_t c2_raddr = 1024UL * 1024 * 1024;
const uint64_t c2_size = (2 << 20);
const int c2_slots = c2_size / c2_line_size;

std::vector<uint64_t> node_list;

// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
using C1 = DirectCache<0,c1_raddr,0,c1_slots,c1_line_size,0>;
using C2 = DirectCache<c1_slots,c2_raddr,(1ULL<<30),c2_slots,c2_line_size,1>;

using C1R = CacheReq<C1>;
using C2R = CacheReq<C2>;

// CLS 2MB
const uint64_t eles = c2_line_size / sizeof(arc_t);
const uint64_t n_blocks = M_arc / eles;

void setup() {
  printf("size node: %lu\n", sizeof(node_t));
  printf("size arc:  %lu\n", sizeof(arc_t));
  node = (node_t *) C1R::alloc(sizeof(node_t) * N_node);
  arc = (arc_t *) C2R::alloc(sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    node_t *nodei = C1R::get_mut<node_t>(node + i);
    nodei->number = i;
    // nodei->firstin = arc + dist2(g);
    // nodei->firstout = arc + dist2(g);
  }

  node_list.reserve(N_node);
  for (uint64_t i = 0; i < N_node; ++ i) {
    node_list[i] = i;
  }
  shuffle(node_list.begin(), node_list.end(), g);

  for (int j = n_blocks - 1; j >= 0; j-- ) {
    // printf("%d, %lx\n", j, (uintptr_t) (arc + j*eles));
    arc_t *p = C2R::get_mut<arc_t>(arc + j * eles);
    for( int i = 0; i < eles; i++ ) { 
      p[i].tail = node;
      p[i].head = node;
    }
  }
}

const int n_ahead = 11;

// TODO: node_t and arc_t
void visit() {
  uint64_t gap1 = 0, gap2 = 0;
  for (int k = 0; k < 4; ++ k) {
    uint64_t t0 = microtime();

    uint64_t mofst = (k & 0x1) * M_arc / 2 / eles;

    for (int j = 0; j < n_blocks/2; j++ ) {
      arc_t *p = C2R::get_mut<arc_t>(arc + (j + mofst) * eles);
      for( int i = 0; i < eles; i++ ) {
        foo_seq(p + i);
      }
    }

    uint64_t t1 = microtime();
    gap1 += t1 - t0;

    // random
    for (int i = 0; i < N_node; ++ i) {
      node_t *ai = C1R::get_mut<node_t, 3, 4>(node + node_list[N_node - 1 - i]);
      node_t *bi = C1R::get_mut<node_t, 3, 4>(node + node_list[i]);
      computation(ai, bi);
    }
    uint64_t t2 = microtime();
    gap2 += t2 - t1;
  }

  printf("g1 %.5f, g2 %.5f\n", gap1/1e6, gap2/1e6);
}

uint64_t t0;
void do_work() {

  setup();
  printf("After setup\n");
  uint64_t start = microtime();
  visit();
  uint64_t end = microtime();
  printf("Visit start at: %.5f s\n", (start - t0)/1e6);
  printf("Exec time %.5f s\n", (end - start)/1e6);
  printf("Dont opt this %d\n", g_payload[5]);
  printf("node miss/hit = %lu, %lu\n", counters[3], counters[4]);
  // check();
}

int main () {
  t0 = microtime();
  init_client();

  do_work();
  return 0;
}
