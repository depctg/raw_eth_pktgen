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

int g_payload[23];

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

static std::mt19937 g(seed);
// static std::uniform_int_distribution<int> dist1(0, N_node-1);
static std::normal_distribution<> dist1(N_node/2, N_node/8);
static std::uniform_int_distribution<int> dist2(0, M_arc-1);

static inline int nextRand() {
  long idx = std::round(dist1(g));
  while (idx < 0 || idx >= N_node)
    idx = std::round(dist1(g));
  return idx;
}

void setup();
void visit();
void check();

static inline void foo_seq(arc_p a) {
  a->tail = node + nextRand();
  a->head = node + nextRand();
  for (int i = 0; i < 8; ++ i) {
    a->payload[i] += (a->head - a->tail);
  }
}

static inline void computation(node_p a, node_p b) {
  for (int i = 0; i < 22; ++ i) {
    a->payload[i] += b->payload[i % 8];
  }
  memcpy(g_payload, a->payload, sizeof(int) * 23);
}
