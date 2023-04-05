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
std::vector<uint64_t> node_list;

void setup() {
  node = (node_t *) aligned_alloc(4096, sizeof(node_t) * N_node);
  arc = (arc_t *) aligned_alloc(4096, sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    node[i].number = i;
    // node[i].firstin = arc +  dist2(g);
    // node[i].firstout = arc + dist2(g);
  }

  node_list.reserve(N_node);
  for (uint64_t i = 0; i < N_node; ++ i) {
    node_list[i] = i;
  }
  shuffle(node_list.begin(), node_list.end(), g);

  for (int i = M_arc - 1; i >= 0; -- i) {
    // uint64_t ti = node_list[nextRand()];
    // tail_dist[i] = ti;
    // uint64_t hi = node_list[nextRand()];
    // head_dist[i] = hi;

    arc[i].tail = node;
    arc[i].head = node;
  }
}

void visit() {
  // seq access
  uint64_t gap1 = 0, gap2 = 0;
  for (int k = 0; k < 4; ++ k) {
    uint64_t t0 = microtime();

    // seq
    uint64_t mofst = (k & 0x1) * M_arc / 2;
    for (int i = 0; i < M_arc / 2; ++ i) {
      foo_seq(arc + i + mofst);
    }

    uint64_t t1 = microtime();
    gap1 += t1 - t0;

    // random 
    for (int i = 0; i < N_node; ++ i) {
      computation(
        node + node_list[N_node - i - 1], 
        node + node_list[i]
      );
    }

    uint64_t t2 = microtime();
    gap2 += t2 - t1;
  }

  printf("g1 %.5f, g2 %.5f\n", gap1/1e6, gap2/1e6);
  // for( int i = 0; i < M_arc; i++ )
  // {
  //   arc[i].nextout = arc[i].tail->firstout;
  //   arc[i].tail->firstout = arc + i;
  //   computation(arc+i, arc[i].tail);

  //   arc[i].nextin = arc[i].head->firstin;
  //   arc[i].head->firstin = arc + i;
  //   computation(arc+i, arc[i].head);
  // }
}

void check() {
  printf("no check\n");
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
