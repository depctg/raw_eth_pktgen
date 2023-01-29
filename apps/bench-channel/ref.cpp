#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "workload.hpp"

void setup() {
  // cache_token_t token = cache_requ
  for (int i = 0; i < M; ++ i) {
    int nexti = nextRand();
    if (nexti == i)
      nexti = (nexti + 1) % M;
    dat[i].next = dat + nexti;

    int previ = nextRand();
    if (previ == i)
      previ = (previ + 1) % M;
    dat[i].prev = dat + previ;

    dat[i].i = i;
    dat[i].j = i * i;
    dat[i].hit = 1;
    dat[i].x = i;
    dat[i].y = -1 * i;
  }
}

void obv_work(arc_t *arc, unsigned n) {
  while (n) {
    dprintf("%ld: %lu %lu %u", arc-dat, arc->i, arc->j, arc->hit);
    // printf("%ld: %lu %lu %u\n", arc-dat, arc->i, arc->j, arc->hit);
    arc->i *= n;
    arc->j -= n;
    arc->hit ++;
    arc = arc->next;
    n--;
  }
}

void seq_work(arc_t *arc, unsigned n) {
  for (unsigned i = 0; i < n; ++ i) {
  dprintf("%d: %lu %lu %u %d %d", i,  arc[i].i, 
                                      arc[i].j, 
                                      arc[i].hit, 
                                      arc[i].x, 
                                      arc[i].y);
    arc[i].x *= arc[i].hit | i;
    arc[i].y *= arc[i].hit ^ i;
    arc[i].hit ++ ;
  }
}

void check() {
  for (int i = 0; i < M; ++ i) {
  dprintf("%d: %lu %lu %u %d %d", i,  dat[i].i, 
                                       dat[i].j, 
                                       dat[i].hit, 
                                       dat[i].x, 
                                       dat[i].y);
    checksum += dat[i].x + dat[i].y;
    checksum += dat[i].i + dat[i].j;
    checksum += dat[i].hit;
  }
  printf("checksum: %lu\n", checksum);
}

int main(int argc, char **argv) {
  srand(seed);
  int it = atoi(argv[1]);
  dat = (arc_t *) calloc(M, sizeof(arc_t));

  do_work();

  check();

}
