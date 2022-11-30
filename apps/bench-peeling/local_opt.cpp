#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "cache.h"
#include "helper.h"
#include "side_channel.h"
#include "workload.hpp"

void setup() {
  unsigned channel = channel_create(
    (uint64_t) dat, M, sizeof(arc_t),
    sizeof(arc_t), 32, 32, 0, 0, CHANNEL_STORE
  );
  for (int i = 0; i < M; ++ i) {
    arc_t* dat_i = (arc_t*) channel_access(channel, i);

    int nexti = nextRand();
    if (nexti == i)
      nexti = (nexti + 1) % M;
    dat_i->next = dat + nexti;

    int previ = nextRand();
    if (previ == i)
      previ = (previ + 1) % M;
    dat_i->prev = dat + previ;

    dat_i->i = i;
    dat_i->j = i * i;
    dat_i->hit = 1;
    dat_i->x = i;
    dat_i->y = -1 * i;
  }
  channel_destroy(channel);
}

typedef struct {
  unsigned hit;
  int x;
  int y;
} arc_x_y_hit_t;

void seq_work_small(arc_t *arc, unsigned n) {
  unsigned channel = channel_create(
    (uint64_t) arc, n, sizeof(arc_t),
    sizeof(arc_x_y_hit_t), 86, 86, 0, 3, CHANNEL_STORE
  );
  for (unsigned i = 0; i < n; ++ i) {
    arc_x_y_hit_t *arc_i = (arc_x_y_hit_t*) channel_access(channel, i);
    dprintf("%d: %d %d %u", i, arc_i->x, 
                               arc_i->y, 
                               arc_i->hit);
    arc_i->hit ++ ;
    arc_i->x *= arc_i->hit | i;
    arc_i->y *= arc_i->hit ^ i;
  }
  channel_destroy(channel);
}

typedef struct {
  char payload[16];
  uint64_t i;
  uint64_t j;
  struct arc* next; 
  struct arc* prev;
  unsigned hit;
} arc_except_x_y_t;

void seq_work_large(arc_t *arc, unsigned n) {
  unsigned channel = channel_create(
    (uint64_t) arc, n, sizeof(arc_t),
    sizeof(arc_except_x_y_t), 32, 32, 0, 5, CHANNEL_STORE
  );
  for (unsigned i = 0; i < n; ++ i) {
    arc_except_x_y_t* arc_i = (arc_except_x_y_t*) channel_access(channel, i);
    dprintf("%d: %lu %lu %u", i,  arc_i->i, 
                                  arc_i->j, 
                                  arc_i->hit);
    arc_i->hit ++ ;
    arc_i->i += arc_i->prev - arc;
    arc_i->j -= arc - arc_i->next;
    unsigned sx = 0, sy = 0;
    for (int k = 0; k < 8; ++ k) {
      arc_i->payload[k] = (arc_i->i + i + k) % 127;
      sx += arc_i->payload[k];
      arc_i->payload[k+8] = (arc_i->j + i * k) % 127;
      sy += arc_i->payload[k+8];
    } 
    arc_i->i += sx;
    arc_i->j -= sy;
  }
  channel_destroy(channel);
}

void check() {
  unsigned channel = channel_create(
    (uint64_t) dat, M, sizeof(arc_t),
    sizeof(arc_t), 32, 32, 0, 0, CHANNEL_LOAD
  );
  for (int i = 0; i < M; ++ i) {
    arc_t* dat_i = (arc_t*) channel_access(channel, i);
    dprintf("%d: %lu %lu %u %d %d", i,  dat_i->i, 
                                        dat_i->j, 
                                        dat_i->hit, 
                                        dat_i->x, 
                                        dat_i->y);
    checksum += dat_i->x + dat_i->y;
    checksum += dat_i->i + dat_i->j;
    checksum += dat_i->hit;
  }
  printf("checksum: %lu\n", checksum);
  channel_destroy(channel);
}

int main(int argc, char **argv) {
  init_client();
  cache_init();
  channel_init();

  int it = atoi(argv[1]);
  dat = (arc_t *) _disagg_alloc(2, sizeof(arc_t) * M);

  do_work();

  check();

}
