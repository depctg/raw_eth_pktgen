#include "workload.hpp"
#include "pattern_generator.hpp"
#include "common.h"

void setup() {
  arcs = (arc_t *) malloc(sizeof(arc_t) * M_arc);
}

int main () {
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
    arcs[i].nextout = arcs + out;
    arcs[i].nextin = arcs + in;
  }
  printf("After prewarm\n");

  // real access
  uint64_t start_us = microtime();

  for (size_t i = 0; i < N_access; ++i) {
    arcs[i % M_arc].ident += i;
    arcs[i % M_arc].nextout->ident = zipf(randrand, M_arc - 1);
    arcs[i % M_arc].nextin->ident = zipf(randrand, M_arc - 1);
  }

  uint64_t end_us = microtime();

  printf("Time %f s\n", (end_us - start_us) / 1e6);

  for (size_t i = 0; i < M_arc; ++ i) {
    checksum += arcs[i].ident;
  }
  printf("Checksum %lu\n", checksum);

  return 0;

}