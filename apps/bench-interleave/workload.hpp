#include <random>
#include <cstdio>

#include "common.h"
#include "util.hpp"

typedef long flow_t;
typedef long cost_t;

typedef struct node node_t;
typedef struct node *node_p;

typedef struct arc arc_t;
typedef struct arc *arc_p;

struct node
{
  node_p child, parent;
  arc_p firstout, firstin;
  int number;

  int payload[23];
};

int g_payload[7];

struct arc
{
  node_p tail, head;
  arc_p nextout, nextin;
  int payload[8];
};

int a = sizeof(arc_t);
int b = sizeof(node_t);

node_t *node;
arc_t *arc;

#define N_node (8 << 20)
#define M_arc (64 << 20)

// #define N_node (8 << 10)
// #define M_arc (64 << 10)

static uint64_t seed = 0x23333;
static uint64_t checksum = 0xdeadbeaf;

static inline int nextRand(int M) {

  seed = ((seed * 7621) + 1) % M;

  // static std::mt19937 g(seed);
  // static std::uniform_int_distribution<int> dist(0, M-1);
  // seed = dist(g);

  // printf("%d\n", (int)r);
  return (int)seed;
}

void setup();
void visit();
void check();

void computation(arc_p a, node_p n) {
  for (int i = 0; i < 22; ++ i) {
    n->payload[i] += a->payload[i % 8];
  }
  memcpy(g_payload, n->payload, sizeof(int) * 7);
}
