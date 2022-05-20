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

/* Basic memory block - a cache line */
typedef struct Block
{
  uint64_t rbuf_offset;
  uint64_t tag;
  struct Block *prev, *next;
  uint64_t wr_id;
  uint32_t sid;
  uint16_t weight;
  uint8_t present;
  uint8_t dirty;
} Block;

/* create new block */
Block *newBlock(uint64_t offset, uint64_t tag, uint8_t present, uint8_t dirty, uint64_t wr_id, uint32_t sid, uint16_t weight);

typedef struct HashStruct
{
	int tag;
	Block *bptr;
	UT_hash_handle hh;
} HashStruct;

HashStruct *new_entry(uint64_t tag, Block *bptr);

/* Doubly linked list of memory blocks */
typedef struct BlockDLL
{
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
void protect(BlockDLL *dll, Block *b);
void dllPrint(Block *head);

#ifdef __cplusplus
}
#endif
#endif