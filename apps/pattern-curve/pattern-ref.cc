#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <algorithm>    // std::shuffle

#include "common.h"
#include "workload.hpp"
#include "util.hpp"

void setup() {
  arc = (arc_t *) malloc(sizeof(arc_t) * M_arc);
  node = (node_t *) malloc(sizeof(node_t) * N_node);

  for (int i = 0; i < N_node; ++ i) {
    node[i].number = i;
  }

  for (uint64_t i = 0; i < N_node; ++ i) {
    node_list[i] = i;
  }
  std::shuffle(node_list, node_list + N_node, g);


  for (int i = 0; i < M_arc; ++ i) {
    arc[i].head = node + node_list[nextRand()];
  }
}

// TODO: node_t and arc_t
// add prefetch ?

void visit() {
#if 1
  for (int i = 0; i < N_node; ++ i) {
    // node_t *ni = node + node_list[nextRand()];
    node_t *ni = node + nextRand();
    // node_t *ni = node + (i % 16384);
    ni->firstin = arc + i;
    computation_node(ni);
  }
#else
  for (int i = 0; i < M_arc; ++ i) {

  }
#endif
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
  // check();
}


int main () {
  t0 = microtime();
  do_work();
  return 0;
}
