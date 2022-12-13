#include <random>
#include <cstdio>
#include "common.h"
#include "pattern_generator.hpp"

typedef long flow_t;
typedef long cost_t;

typedef struct node node_t;
typedef struct node *node_p;

typedef struct arc arc_t;
typedef struct arc *arc_p;

struct node
{
  cost_t potential; 
  int orientation;
  node_p child;
  node_p pred;
  node_p sibling;
  node_p sibling_prev;     
  arc_p basic_arc; 
  arc_p firstout, firstin;
  arc_p arc_tmp;
  flow_t flow;
  long depth; 
  int number;
  int time;

  char padding[24];
};


struct arc
{
  cost_t cost;
  node_p tail, head;
  int ident;
  arc_p nextout, nextin;
  flow_t flow;
  cost_t org_cost;
};

int a = sizeof(arc_t);
int b = sizeof(node_t);

node_t *node;
arc_t *arc;

#define N_node (32 << 20)
#define M_arc (64 << 20)

static uint64_t seed = 0x23333;
static uint64_t checksum = 0xdeadbeaf;

#define randrand 0.4
#define rand 0.8
#define mrand 1.3

static inline int nextRand(int M) {

  // seed = ((seed * 7621) + 1) % M;

  // static std::mt19937 g(seed);
  // static std::uniform_int_distribution<int> dist(0, M-1);
  // seed = dist(g);

  // printf("%d\n", (int)r);
  // return (int)seed;
  return zipf(randrand, M) - 1;
}

extern void setup();
extern void visit();
extern void check();


void do_work() {
  setup();

  uint64_t start = microtime();
  visit();
  uint64_t end = microtime();

  printf("Exec time %lu us\n", end - start);
  check();
}