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
  int max_weight;
  struct BlockDLL **weight_dll;
} Victim;

Victim *initVictim(int w);

void add_to_victim(Victim *v, Block *b);
void remove_from_victim(Victim *v, Block *b);
void inspect(Victim *v);

typedef struct Policy {
  int max_weight;
  Victim *v;

  void (*fresh_add)(struct Policy *, Block *b);
  void (*access_existing)(struct Policy *, Block *b);
  Block *(*pop_victim)(struct Policy *);
} Policy;

Policy *initReplacement(int max_weight, \
                        void (*fresh_add)(struct Policy *, Block *b), \
                        void (*access_existing)(struct Policy *, Block *b), \
                        Block *(*pop_victim)(struct Policy *));

void add_to_LRU(Policy *p, Block *b);
void touch_LRU(Policy *p, Block *b);
Block *pop_LRU(Policy *p);

void add_to_LFU(Policy *p, Block *b);
void touch_LFU(Policy *p, Block *b);
Block *pop_LFU(Policy *p);

#ifdef __cplusplus
}
#endif

#endif