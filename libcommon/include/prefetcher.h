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

// poll awaiting prefetches until wr_id
// if wr_id is 0, poll all pending blocks
void pollAwait(uint64_t wr_id, Inflights *ins, BlockDLL *dll, Ambassador *a);
void printAwaits(Inflights *ins);

#ifdef __cplusplus
}
#endif

#endif