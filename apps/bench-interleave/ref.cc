#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "workload.hpp"
#include "unistd.h"
#include "util.hpp"

void setup() {
  node = (node_t *) aligned_alloc(4096, sizeof(node_t) * N_node);
  arc = (arc_t *) aligned_alloc(4096, sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    node[i].number = -i;
    node[i].firstin = arc + nextRand(M_arc);
    node[i].firstout = arc + nextRand(M_arc);
  }

  for (int i = 0; i < M_arc; ++ i) {
    arc[i].tail = node + nextRand(N_node);
    arc[i].head = node + nextRand(N_node);
  }
}

void visit() {
  for( int i = 0; i < M_arc; i++ )
  {
    arc[i].nextout = arc[i].tail->firstout;
    arc[i].tail->firstout = arc + i;
    arc[i].nextin = arc[i].head->firstin;
    arc[i].head->firstin = arc + i;
  }
}

void check() {
  printf("no check\n");
}

void do_work() {
  setup();

  uint64_t start = microtime();
  visit();
  uint64_t end = microtime();

  printf("Exec time %.5f s\n", (end - start)/1e6);
  // check();
}

int main () {

  do_work();
  return 0;
}
