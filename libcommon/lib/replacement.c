#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "mem_block.h"
#include "replacement.h"

Victim *initVictim(int w) {
	Victim *v = (Victim *) malloc(sizeof(Victim));
  v->max_weight = w;
	v->weight_dll = (BlockDLL **) malloc(sizeof(BlockDLL *) * (w+1));
	for (int i = 0; i <= w; ++i) {
		v->weight_dll[i] = initBlockDLL();
	}
	return v;
}

void add_to_victim(Victim *v, Block *b) {
	BlockDLL *dll = v->weight_dll[b->weight];
	add_to_head(dll, b);
}

void remove_from_victim(Victim *v, Block *b) {
	BlockDLL *dll = v->weight_dll[b->weight];
	remove_from_dll(dll, b);
}

void inspect(Victim *v) {
  for (int i = 0; i <= v->max_weight; ++ i) {
    if (v->weight_dll[i]->head != NULL) {
      printf("Weight %d:\n", i);
      dllPrint(v->weight_dll[i]);
    }
  }
}

Policy *initReplacement(int max_weight, \
                        void (*fresh_add)(struct Policy *, Block *b), \
                        void (*access_existing)(struct Policy *, Block *b), \
                        Block *(*pop_victim)(struct Policy *)) {
  Policy *p = (Policy *) malloc(sizeof(Policy));
  p->max_weight = max_weight;
  p->v = initVictim(max_weight);

  p->fresh_add = fresh_add;
  p->access_existing = access_existing;
  p->pop_victim = pop_victim;
  return p;
}

void add_to_LRU(Policy *p, Block *b) {
  if (b->weight != 0)
    b->weight = 0;
  add_to_victim(p->v, b);
}

void touch_LRU(Policy *p, Block *b) {
  if (b->weight != 0)
    b->weight = 0;
  touch(p->v->weight_dll[0], b);
}

Block *pop_LRU(Policy *p) {
  return pop_last(p->v->weight_dll[0]);
}

void add_to_LFU(Policy *p, Block *b) {
  b->weight = (b->weight < p->max_weight ? b->weight : p->max_weight);
  add_to_victim(p->v, b);
}

void touch_LFU(Policy *p, Block *b) {
  remove_from_victim(p->v, b);
  b->weight = (b->weight + 1 < p->max_weight ? b->weight + 1 : p->max_weight);
  add_to_victim(p->v, b);
}

Block *pop_LFU(Policy *p) {
  for (int i = 0; i <= p->max_weight; ++i) {
    Block *vic = pop_last(p->v->weight_dll[i]);
    if (vic != NULL) 
      return vic;
  }
  return NULL;
}