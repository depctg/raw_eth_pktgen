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

const uint64_t line_size = (4UL << 10);

const uint64_t c1_raddr = 0;
const uint64_t c1_size = (512<< 20);
const int c1_slots = c1_size / line_size;

// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
// using C1 = DirectCache<0,c1_raddr,0,c1_slots,line_size,0>;
using C1 = FullLRUCache<0,c1_raddr,0,c1_slots,line_size,0>;

using C1R = CacheReq<C1>;
uint64_t *nano_get_lat;

// CLS 4K
const uint64_t eles = line_size / sizeof(arc_t);
const uint64_t n_blocks = M_arc / eles;

void setup() {
  node = (node_t *) C1R::alloc(sizeof(node_t) * N_node);
  arc = (arc_t *) C1R::alloc(sizeof(arc_t) * M_arc);

  printf("node: %p, arc: %p\n", node, arc);
  nano_get_lat = (uint64_t *) aligned_alloc(4096, 2 * M_arc * sizeof(uint64_t));

  for (int i = 0; i < N_node; ++ i) {
    node_t *nodei = C1R::get_mut<node_t>(node + i);
    nodei->number = i;
    nodei->firstin = arc + dist2(g);
    nodei->firstout = arc + dist2(g);
  }

  for (uint64_t i = 0; i < N_node; ++ i) {
    node_list[i] = i;
  }
  std::shuffle(node_list, node_list + N_node, g);

  for (int j = M_arc - 1; j >= 0; j-- ) {
    // printf("%d, %lx\n", j, (uintptr_t) (arc + j*eles));
    arc_t *p = C1R::get_mut<arc_t>(arc + j);
    p->tail = node + node_list[nextRand()];
    p->head = node + node_list[nextRand()];
  }
}

// TODO: node_t and arc_t
// add prefetch ?

void visit() {
  for (int i = 0; i < M_arc; ++ i) {
    uint64_t arc_get_start = getCurNs(); 
    arc_t *arci = C1R::get_mut<arc_t>(arc + i); 
    uint64_t arc_get_end = getCurNs();
    node_t *node_head = C1R::get_mut<node_t>(arci->head);
    uint64_t node_get_end = getCurNs();
    nano_get_lat[i * 2] = arc_get_end - arc_get_start;
    nano_get_lat[i * 2 + 1] = node_get_end - arc_get_end;

    arci->nextin = node_head->firstin;
    node_head->firstin = arc + i;
    computation(arci, node_head);
  }
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
  printf("arc miss/hit = %lu, %lu\n", counters[1], counters[2]);
  printf("node miss/hit = %lu, %lu\n", counters[3], counters[4]);

  printf("Dump get latency\n");
  FILE *lf = fopen("lru_get_lat.txt", "wb");
  fwrite(nano_get_lat, sizeof(uint64_t), 2*M_arc, lf);
  fclose(lf);
  // check();
}


int main () {
  t0 = microtime();
  init_client();

  do_work();
  return 0;
}
