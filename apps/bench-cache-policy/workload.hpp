#include <random>
#include <cstdio>
#include "common.h"

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


arc_t *arcs;

#define M_arc (8ULL << 20) // working set = M_arc * 64 bytes

// distinguish rand / seq
#define N_access (8ULL << 20)

#define rseed 2333

#define randrand 0.4
#define rand 0.8
#define mrand 1.0

static uint64_t checksum = 0xdeadbeaf;