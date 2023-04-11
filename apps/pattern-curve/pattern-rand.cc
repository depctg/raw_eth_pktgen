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

const uint64_t line_size = (128);

const uint64_t c1_raddr = 0;
const uint64_t c1_size = (116ULL<< 20);
const int c1_slots = c1_size / line_size;

// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
using C1 = DirectCache<0,c1_raddr,0,c1_slots,line_size,0>;
// using C1 = SetAssocativeCache<0,c1_raddr,0,c1_slots,line_size,0,4>;

using C1R = CacheReq<C1>;

uint64_t lat_sum = 0;

void setup() {
  node = (node_t *) C1R::alloc(sizeof(node_t) * N_node);

  for (int i = 0; i < N_node; ++ i) {
    node_t *nodei = C1R::get_mut<node_t>(node + i);
    nodei->number = i;
  }
  for (uint64_t i = 0; i < N_node; ++ i) {
    node_list[i] = i;
  }
  std::shuffle(node_list, node_list + N_node, g);
}

// TODO: node_t and arc_t
// add prefetch ?

void visit() {

  for (int i = 0; i < N_node; ++ i) {
    uint64_t k = node_list[nextRand()];
    uint64_t start = getCurNs();
    node_t *node_head = C1R::get_mut<node_t>(node + k);
    uint64_t end = getCurNs();
    lat_sum += end - start;
    node_head->firstin = arc + i;
    computation_node(node_head);
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
  printf("get sum: %.6f s\n", lat_sum/1e9);
  // check();
}


int main () {
  t0 = microtime();
  init_client();

  do_work();
  return 0;
}
