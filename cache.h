#ifndef __LRUCACHE__
#define __LRUCACHE__
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct Block
{
	uint64_t offset;
	struct Block *prev, *next;
	uint8_t dirty;
} Block;
Block *newBlock(uint8_t dirty, uint64_t offset);

// LRU appears at the rear
typedef struct BlockDLL
{
	Block *head, *tail;
} BlockDLL;
BlockDLL *initBlockDLL();
// pop last
uint64_t popVictim(BlockDLL *dll);
// add front
void addHot(BlockDLL *dll, Block *b);
// refresh life time of visited victim
void touch(BlockDLL *dll, Block *b);

// A queue that represents free slots in rbuf
typedef struct FreeQueue
{
	uint32_t capacity;
	uint32_t front, end, frees;
	uint64_t* slots;
} FreeQueue;
FreeQueue *initQueue(uint64_t max_size, uint32_t cache_line_size);
int isFull(FreeQueue *fq);
int isEmpty(FreeQueue *fq);
// claim a block, will evit LRU if needed
uint64_t claim(FreeQueue *fq, BlockDLL *dll);
void reclaim(FreeQueue *fq, uint64_t offset);

typedef struct CacheTable
{
	uint64_t tag_mask;
	uint64_t addr_mask;
	uint64_t max_size;
	uint32_t cache_line_size;

	void *reqs;
	char *line_pool;

	// a hashmap of blocks
	Block **map;
	// a dll of blocks
	BlockDLL *dll;
	// a queue of free slots
	FreeQueue *fq;
} CacheTable;

// max_size, cache_line_size (byte)
CacheTable *createCacheTable(
	uint64_t max_size,
	uint32_t cache_line_size,
	void *req_buffer,
	void *recv_buffer
);
// insert from outside of rbuf?
// void insert(uint64_t addr);

char *cache_access(CacheTable *table, uint64_t addr);

#ifdef __cplusplus
}
#endif
#endif