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

#define CHECK_NODE_DIST 1

uint64_t head_dist[M_arc];
uint64_t tail_dist[M_arc];

void setup() {
  node = (node_t *) aligned_alloc(4096, sizeof(node_t) * N_node);
  arc = (arc_t *) aligned_alloc(4096, sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    node[i].number = -i;
    node[i].firstin = arc +  dist2(g);
    node[i].firstout = arc + dist2(g);
  }

  std::vector<uint64_t> node_list;
  node_list.reserve(N_node);
  for (uint64_t i = 0; i < N_node; ++ i) {
    node_list[i] = i;
  }
  shuffle(node_list.begin(), node_list.end(), std::default_random_engine(seed));

  for (int i = 0; i < M_arc; ++ i) {
    uint64_t ti = node_list[nextRand()];
    tail_dist[i] = ti;
    uint64_t hi = node_list[nextRand()];
    head_dist[i] = hi;

    arc[i].tail = node + ti;
    arc[i].head = node + hi;
  }
}

void visit() {
  for( int i = 0; i < M_arc; i++ )
  {
    arc[i].nextout = arc[i].tail->firstout;
    arc[i].tail->firstout = arc + i;
    computation(arc+i, arc[i].tail);

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
  fwrite(head_dist, sizeof(uint64_t), M_arc, hf);
  fclose(hf);

  FILE *tf = fopen("tail_dist.txt", "wb");
  fwrite(tail_dist, sizeof(uint64_t), M_arc, tf);
  fclose(tf);
#endif
  return 0;
}
