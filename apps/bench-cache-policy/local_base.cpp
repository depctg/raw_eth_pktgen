#include "common.h"
#include "cache.h"
#include "workload.hpp"
#include "pattern_generator.hpp"

void setup() {
  init_client();
  cache_init();

  arcs = (arc_t *) _disagg_alloc(2, sizeof(arc_t) * M_arc);
}

int main() {
  setup();
  rand_val(rseed);

  // pre-warm
  for (size_t i = 0; i < M_arc; ++ i) {
    int out = zipf(randrand, M_arc) - 1;
    int in = zipf(randrand, M_arc) - 1;
    // if (out >= M_arc || in >= M_arc) {
    //   fprintf(stderr, "zipf out of bound\n");
    //   exit(0);
    // }
    cache_token_t tk = cache_request((uint64_t) (arcs + i));
    arc_t *arci = (arc_t *) cache_access_mut(tk);

    arci->nextout = arcs + out;
    arci->nextin = arcs + in;
  }
  printf("After prewarm\n");

  // real access
  uint64_t start_us = microtime();

  for (size_t i = 0; i < N_access; ++i) {
    // cache_token_t tk = cache_request((uint64_t) (arcs + (i % M_arc)));
    // arc_t *arci = (arc_t *) cache_access_mut(tk);

    arc_t *arci = (arc_t *) cache_access_mod_opt_mut(arcs + (i % M_arc));
    arci->ident += i;

    // cache_token_t tkout = cache_request((uint64_t) (arci->nextout));
    // arc_t *arcout = (arc_t *) cache_access_mut(tkout);
    arc_t *arcout = (arc_t *) cache_access_mod_opt_mut(arci->nextout);
    arcout->ident = zipf(randrand, M_arc - 1);
    
    // cache_token_t tkin = cache_request((uint64_t) (arci->nextin));
    // arc_t *arcin = (arc_t *) cache_access_mut(tkin);

    arc_t *arcin = (arc_t *) cache_access_mod_opt_mut(arci->nextin);
    arcin->ident = zipf(randrand, M_arc - 1);
  }

  uint64_t end_us = microtime();

  printf("Time %f s\n", (end_us - start_us) / 1e6);

  for (size_t i = 0; i < M_arc; ++ i) {
    cache_token_t tk = cache_request((uint64_t) (arcs + i));
    arc_t *arci = (arc_t *) cache_access_mut(tk);
    checksum += arci->ident;
  }
  printf("Checksum %lu\n", checksum);

  return 0;


}