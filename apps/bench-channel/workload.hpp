#include <random>
#include <cstdio>
#include "common.h"

typedef struct arc {
  char payload[16];
  uint64_t i;
  uint64_t j;
  struct arc* next; 
  struct arc* prev;
  unsigned hit;
  int x;
  int y;
} arc_t;

static arc_t* dat;

#define M 4194304
// #define M 1024

static uint64_t seed = 0x23333;

static uint64_t checksum = 0xdeadbeaf;

static inline int nextRand() {

  seed = ((seed * 7621) + 1) % M;

  // static std::mt19937 g(seed);
  // static std::uniform_int_distribution<int> dist(0, M-1);
  // seed = dist(g);

  // printf("%d\n", (int)r);
  return (int)seed;
}

extern void setup();
extern void seq_work(arc_t *arc, unsigned n);
extern void obv_work(arc_t *arc, unsigned n);
extern void check();

void do_work() {
  uint64_t s0 = getCurNs();
  setup();
  uint64_t s1 = getCurNs();

  // for (int i = 0; i < it; ++ i) {
  //   // oblivious
  for (int i = 0; i < M / 64; ++ i) {
    obv_work(dat + nextRand(), 64);
  }
  uint64_t s2 = getCurNs();

  //   // sequantial
  //   work2(dat + (M/2 - 1) * (i % 2), M/2);
  // }
  seq_work(dat + (M/2 - 1), M/2);
  uint64_t s3 = getCurNs();

  printf("Exe time total %f s\n%f s\n%f s\n%f s\n", 
    (s3 - s0)/(float)1e9,
    (s1 - s0)/(float)1e9,
    (s2 - s1)/(float)1e9,
    (s3 - s2)/(float)1e9
  );
}