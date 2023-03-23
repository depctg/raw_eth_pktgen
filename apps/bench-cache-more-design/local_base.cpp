#include "workload.hpp"
#include "pattern_generator.hpp"
#include "common.h"
#include "cache.h"

void setup() {
  init_client();
  cache_init();

  nodes = (node_t *) _disagg_alloc(2, sizeof(node_t) * N_node);
  arcs = (arc_t *) _disagg_alloc(3, sizeof(node_t) * M_arc);
}

int main () {
  setup();
  rand_val(rseed);

  // sequantial access arc
  // randome access node
  // zipf arc->head = 0.4 (no hot spot)
  // zipf arc->tail = 1.3 (has hot spot)

  // pre-warm
  for (size_t i = 0; i < N_node; ++ i) {
    cache_token_t tk = cache_request((uint64_t) nodes + i);
    node_t *nodei = (node_t *) cache_access_mut(tk);
    nodei->depth = i;
  }

  for (size_t i = 0; i < M_arc; ++ i) {
    cache_token_t tk = cache_request((uint64_t) arcs + i);
    arc_t *arci = (arc_t *) cache_access_mut(tk);

    int h = zipf(randrand, N_node) - 1;
    arci->head = nodes + h;

    int t = zipf(mrand, N_node) - 1;
    arci->tail = nodes + t;
  }
  printf("After prewarm\n");

  // real access
  uint64_t start_us = microtime();

  for (size_t i = 0; i < N_access; ++i) {
    cache_token_t tka = cache_request((uint64_t) arcs + (i % M_arc));
    arc_t *arci = (arc_t *) cache_access_nrtc_mut(tka);
    arci->ident += i;

    cache_token_t tkn = cache_request((uint64_t) nodes + i);
    node_t *nodei = (node_t *) cache_access_mut(tkn);
    // arcs[i % M_arc].ident += i;
    // arcs[i % M_arc].head->depth += zipf(randrand, N_node);
    // arcs[i % M_arc].tail->depth += zipf(randrand, N_node);
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