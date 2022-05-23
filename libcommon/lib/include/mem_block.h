#ifndef __MEM_BLOCK__
#define __MEM_BLOCK__
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

enum BLOCK_STATUS {present, pending, absent};

/* Basic memory block - a cache line */
typedef struct Block {
  uint64_t rbuf_offset;
  uint64_t tag;
  struct Block *prev, *next;
  uint64_t wr_id;
  uint16_t weight;
  enum BLOCK_STATUS status;
  uint8_t dirty;
} Block;

/* create new block */
Block *newBlock(uint64_t rbuf_offset, uint64_t tag, enum BLOCK_STATUS bs, uint8_t dirty, uint64_t wr_id, uint16_t weight);

typedef struct HashBlock {
	int tag;
	Block *bptr;
	UT_hash_handle hh;
} HashBlock;

HashBlock *new_entry(uint64_t tag, Block *bptr);

/* Doubly linked list of memory blocks */
typedef struct BlockDLL {
  Block *head, *tail;
} BlockDLL;
BlockDLL *initBlockDLL();

// add to head of dll
void add_to_head(BlockDLL *dll, Block *b);
// add after end
void add_to_tail(BlockDLL *dll, Block *b);
// move given block to the head of dll
void touch(BlockDLL *dll, Block *b);
// remote from dll
void remove_from_dll(BlockDLL *dll, Block *b);
void dllPrint(Block *head);

typedef struct Victim {
  BlockDLL **weight_dll;
} Victim;

Victim *initVictim(int range);

void add_to_victim(Victim *v, Block *b);
void remove_from_victim(Victim *v, Block *b);
void inspect(Victim *v);

#ifdef __cplusplus
}
#endif
#endif