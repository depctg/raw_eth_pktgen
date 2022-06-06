#ifndef __REPLACE__
#define __REPLACE__
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "uthash.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Victim {
  uint64_t max_weight;
  struct BlockDLL **weight_dll;
} Victim;

Victim *initVictim(uint64_t w);

void add_to_victim(Victim *v, Block *b);
void remove_from_victim(Victim *v, Block *b);
void inspect(Victim *v);
void destruct_victim(Victim *v);

typedef struct Policy {
  uint64_t max_weight;
  Victim *v;

  void (*fresh_add)(struct Policy *, Block *b);
  void (*access_existing)(struct Policy *, Block *b);
  Block *(*pop_victim)(struct Policy *);

  uint64_t *future_refs;
  uint64_t *future_dist;
  uint64_t *precomp_evicts;
  uint64_t future_depth;
  uint64_t time_step;
  uint64_t prebuff_step;
} Policy;

Policy *initReplacement(uint64_t max_weight, \
                        void (*fresh_add)(struct Policy *, Block *b), \
                        void (*access_existing)(struct Policy *, Block *b), \
                        Block *(*pop_victim)(struct Policy *));

void add_to_LRU(Policy *p, Block *b);
void touch_LRU(Policy *p, Block *b);
Block *pop_LRU(Policy *p);

void add_to_LFU(Policy *p, Block *b);
void touch_LFU(Policy *p, Block *b);
Block *pop_LFU(Policy *p);


typedef struct HashFuture {
  int ref_tag;
  uint64_t time_step;
  UT_hash_handle hh;
} HashFuture;

void regist_future(Policy *p, uint64_t *refs, uint64_t depth, uint64_t capacity);
void add_to_OPT(Policy *p, Block *b);
void touch_OPT(Policy *p, Block *b);
Block *_pre_comp_pop_OPT(Policy *p);
Block *pop_OPT(Policy *p);

void display_future(Policy *p);

#ifdef __cplusplus
}
#endif

#endif