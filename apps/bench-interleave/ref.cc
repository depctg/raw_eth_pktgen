#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "workload.hpp"


void setup() {
  node = (node_t *) malloc(sizeof(node_t) * N_node);
  arc = (arc_t *) malloc(sizeof(arc_t) * M_arc);

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
  uint64_t check_sum = 0;
  for (int i = 0; i < M_arc; ++ i) {
    check_sum += arc[i].tail->number;
    check_sum += arc[i].head->number;
  }
  printf("Checksum = %lu\n", check_sum);
}

int main () {

  do_work();
  return 0;
}