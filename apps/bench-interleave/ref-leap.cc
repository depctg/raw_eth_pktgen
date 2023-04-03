#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "workload.hpp"
#include "unistd.h"

#define set_pref_check 328
#define set_falt_check 329
#define reset_swap_stat 330

void setup() {
  node = (node_t *) aligned_alloc(4096, sizeof(node_t) * N_node);
  arc = (arc_t *) aligned_alloc(4096, sizeof(arc_t) * M_arc);
  printf("node: %lu\n", (intptr_t) node);
  printf("arc: %lu\n", (intptr_t) arc);
  syscall(set_pref_check, (unsigned long) arc, (unsigned long) (arc + M_arc));
  syscall(set_falt_check, (unsigned long) node, (unsigned long) (node + N_node));

  for (int i = 0; i < N_node; ++ i) {
    node[i].number = -i;
    node[i].firstin = arc + nextRand(M_arc);
    node[i].firstout = arc + nextRand(M_arc);
  }

  for (int i = 0; i < M_arc; ++ i) {
    arc[i].tail = node + nextRand(N_node);
    arc[i].head = node + nextRand(N_node);
  }
  syscall(reset_swap_stat);
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

  printf("Exec time %lu us\n", end - start);
  // check();
}

int main () {

  do_work();
  return 0;
}
