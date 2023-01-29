#include "workload.hpp"
#include "pattern_generator.hpp"
#include "common.h"

void setup() {
  nodes = (node_t *) malloc(sizeof(node_t) * N_node);
  arcs = (arc_t *) malloc(sizeof(arc_t) * M_arc);
}

// Direct cache size = linesize * dblocks 
const size_t dlinesize = 1024;
const size_t dblocks = 1024, dblock_mut = dblocks + 1;
uint64_t dbase = 0, drbase = 0;
const uint64_t dtags[dblocks];
const uint64_t dhotness[dblocks];
static inline void * drequest(uint64_t ptr) {
    uint64_t tag = (ptr / dlinesize)
    uint64_t bucket = tag % dblocks;
    uint64_t lptr = (dbase + bucket * dlinesize)
    if (dtags[bucket] != tag)
        rdma(lptr, dlinesize, drbase + bucket * dlinesize, 0, IBV_WR_RDMA_READ);
    return (void *) lptr;
}
// hotness cache

int main () {
  setup();
  rand_val(rseed);

  // sequantial access arc
  // randome access node
  // zipf arc->head = 0.4 (no hot spot)
  // zipf arc->tail = 1.3 (has hot spot)

  // pre-warm
  for (size_t i = 0; i < N_node; ++ i) {
    nodes[i].depth = i;
  }

  for (size_t i = 0; i < M_arc; ++ i) {
    int h = zipf(randrand, N_node) - 1;
    arcs[i].head = nodes + h;

    int t = zipf(mrand, N_node) - 1;
    arcs[i].tail = nodes + t;
  }
  printf("After prewarm\n");

  // real access
  uint64_t start_us = microtime();

  for (size_t i = 0; i < N_access; ++i) {
    arcs[i % M_arc].ident += i;
    arcs[i % M_arc].head->depth += zipf(randrand, N_node);
    arcs[i % M_arc].tail->depth += zipf(randrand, N_node);
  }

  uint64_t end_us = microtime();

  printf("Time %f s\n", (end_us - start_us) / 1e6);

  for (size_t i = 0; i < N_node; ++i) {
    checksum += nodes[i].depth;
  }
  for (size_t i = 0; i < M_arc; ++ i) {
    checksum += arcs[i].ident;
  }
  printf("Checksum %lu\n", checksum);

  return 0;

}
