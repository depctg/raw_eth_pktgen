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

#define n_blocks 2048
#define eles 32768

void visit() {
  for (int j = 0; j < n_blocks; ++ j) {
    arc_t *base = arc + j * eles;
    for( int i = 0; i < eles; i++ )
    {
      int idx = j * n_blocks + i;
      base[i].nextout = base[i].tail->firstout;
      base[i].tail->firstout = arc + idx;
      base[i].nextin = base[i].head->firstin;
      base[i].head->firstin = arc + idx;
    }
  }
}

void check() {
  printf("no check\n");
}

int main () {

  do_work();
  return 0;
}
