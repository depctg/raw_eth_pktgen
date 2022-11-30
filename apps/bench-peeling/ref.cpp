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

void seq_work_small(arc_t *arc, unsigned n) {
  for (unsigned i = 0; i < n; ++ i) {
  dprintf("%d: %d %d %u", i, arc[i].x, 
                             arc[i].y, 
                             arc[i].hit);
    arc[i].hit ++ ;
    arc[i].x *= arc[i].hit | i;
    arc[i].y *= arc[i].hit ^ i;
  }
}

void seq_work_large(arc_t *arc, unsigned n) {
  for (unsigned i = 0; i < n; ++ i) {
  dprintf("%d: %lu %lu %u", i,  arc[i].i, 
                                arc[i].j, 
                                arc[i].hit);
    arc[i].hit ++ ;
    arc[i].i += arc[i].prev - arc;
    arc[i].j -= arc - arc[i].next;
    unsigned sx = 0, sy = 0;
    for (int k = 0; k < 8; ++ k) {
      arc[i].payload[k] = (arc[i].i + i + k) % 127;
      sx += arc[i].payload[k];
      arc[i].payload[k+8] = (arc[i].j + i * k) % 127;
      sy += arc[i].payload[k+8];
    } 
    arc[i].i += sx;
    arc[i].j -= sy;
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
  int it = atoi(argv[1]);
  dat = (arc_t *) calloc(M, sizeof(arc_t));

  do_work();

  check();

}
