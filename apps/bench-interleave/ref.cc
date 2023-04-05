#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <algorithm>    // std::shuffle

#include "common.h"
#include "workload.hpp"
#include "unistd.h"
#include "util.hpp"

#define CHECK_NODE_DIST 0

uint64_t head_dist[M_arc];
uint64_t tail_dist[M_arc];

void setup() {
  node = (node_t *) aligned_alloc(4096, sizeof(node_t) * N_node);
  arc = (arc_t *) aligned_alloc(4096, sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    node[i].number = i;
    node[i].firstin = arc +  dist2(g);
    node[i].firstout = arc + dist2(g);
  }

  // node_list.reserve(N_node);
  for (uint64_t i = 0; i < N_node; ++ i) {
    node_list[i] = i;
  }
  std::shuffle(&node_list[0], node_list + N_node, g);

  for (int i = 0; i < M_arc; ++ i) {
    uint64_t ti = node_list[nextRand()];
    uint64_t hi = node_list[nextRand()];

#if CHECK_NODE_DIST
    tail_dist[i] = ti;
    head_dist[i] = hi;
#endif

    arc[i].tail = node + ti;
    arc[i].head = node + hi;
  }
}

void visit() {
  for( int i = 0; i < M_arc; i++ )
  {
    // arc[i].nextout = arc[i].tail->firstout;
    // arc[i].tail->firstout = arc + i;
    // computation(arc+i, arc[i].tail);

    arc[i].nextin = arc[i].head->firstin;
    arc[i].head->firstin = arc + i;
    computation(arc+i, arc[i].head);
  }
}

void check() {
  // printf("no check\n");
  uint64_t check_sum = 0;
  for (int i = 0; i < M_arc; ++ i) {
    check_sum += arc[i].tail->number;
    check_sum += arc[i].head->number;
  }
  printf("check: %lx\n", check_sum);
}

void do_work() {
  setup();
  printf("after setup\n");
  uint64_t start = microtime();
  visit();
  uint64_t end = microtime();

  printf("Exec time %.5f s\n", (end - start)/1e6);
  printf("Dont opt this %d\n", g_payload[5]);
  // check();
}

int main () {

  do_work();

#if CHECK_NODE_DIST
  printf("Dumping head tail distributions\n");
  FILE *hf = fopen("head_dist.txt", "wb");
  fwrite(head_dist, sizeof(uint64_t), N_node, hf);
  fclose(hf);

  // FILE *tf = fopen("tail_dist.txt", "wb");
  // fwrite(tail_dist, sizeof(uint64_t), N_node, tf);
  // fclose(tf);

  FILE *lf = fopen("node_list.txt", "wb");
  fwrite(node_list, sizeof(uint64_t), N_node, lf);
  fclose(lf);
#endif
  return 0;
}
