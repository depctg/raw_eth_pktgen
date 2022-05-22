#ifndef __PREFETCHER__
#define __PREFETCHER__

#include "mem_block.h"
#include "ambassador.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AwaitBlock
{
  Block *b;
  struct AwaitBlock *next;
} AwaitBlock;

/* A single linked list of pending blocks */
typedef struct Inflights
{
  AwaitBlock *head, *tail;
} Inflights;

Inflights *initAwaits();

// register block to inflight list
void awaitFetch(Inflights *ins, Block *b);

void printAwaits(Inflights *ins);

#ifdef __cplusplus
}
#endif

#endif