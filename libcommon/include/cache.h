#ifndef __LRUCACHE__
#define __LRUCACHE__
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


typedef struct Block
{
	uint64_t rbuf_offset;
	uint64_t tag;
	struct Block *prev, *next;
	uint8_t present;
	uint8_t dirty;
} Block;
Block *newBlock(uint64_t offset, uint64_t tag, uint8_t present, uint8_t dirty);

typedef struct HashStruct
{
	int tag;
	Block *bptr;
	UT_hash_handle hh;
} HashStruct;

// LRU appears at the rear
typedef struct BlockDLL
{
	Block *head, *tail;
} BlockDLL;
BlockDLL *initBlockDLL();
// add front
void addHot(BlockDLL *dll, Block *b);
// add after end
void addCold(BlockDLL *dll, Block *b);
// refresh life time of visited victim
void touch(BlockDLL *dll, Block *b);

// A queue that represents free slots in rbuf
typedef struct FreeQueue
{
	uint64_t capacity;
	uint32_t front, end, frees;
	uint64_t* slots;
} FreeQueue;
FreeQueue *initQueue(uint64_t max_size, uint32_t cache_line_size);
int isFull(FreeQueue *fq);
int isEmpty(FreeQueue *fq);
// claim a block, return 0 if no free
int claim(FreeQueue *fq, uint64_t *offset);
void reclaim(FreeQueue *fq, uint64_t *offset);

// Centralized communicator
enum {
    CACHE_WRITE = 0,
    CACHE_READ = 1,
};
typedef enum {SUCC, RINGF, SERVF} send_rel;
typedef struct Ambassador
{
	char *reqs;
	char *line_pool;
	uint64_t cache_line_size;
	size_t req_size; /* sizeof(char) * line_size + sizeof(struct req) */

	uint32_t *snid;
	uint32_t ring_size;
	uint32_t front, end, frees;
} Ambassador;
Ambassador *newAmbassador(uint32_t ring_size, uint64_t cls, void *req_buffer, void *recv_buffer);
uint32_t get_sid(Ambassador *a);
void ret_sid(Ambassador *a, uint32_t sid);

void fetch_sync(Block *b, Ambassador *a, uint8_t tag_shifts);
void update_sync(void *dat_buf, uint64_t addr, uint64_t size, Ambassador *a);
// not implemented
send_rel fetch_async(Block *b, Ambassador *a);
uint64_t update_async(Block *b, Ambassador *a);


typedef struct CacheTable
{
	uint64_t tag_mask;
	uint64_t addr_mask;
	uint64_t max_size;
	uint64_t misses;
	uint64_t accesses;

	Ambassador *amba;

	uint32_t cache_line_size;
	uint8_t tag_shifts;

	// a hashmap of blocks
	// Block **map;
	HashStruct *map;
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

// pop LRU, update if dirty
uint64_t popVictim(BlockDLL *dll, CacheTable *table);

/* public interfaces: */

// access arbitrary addr
// return a pointer to real rbuf
char *cache_access(CacheTable *table, uint64_t addr);

// write to cache line indexed by {tag} 
// |<---------- cache line size ---------->|
// | line offset | write {s} size | intact | 
void write_to_CL(CacheTable *table, uint64_t tag, uint64_t line_offset, void *dat_buf, uint64_t s);

// write one byte to arbitrary addr
void cache_write(CacheTable *table, uint64_t addr, void *dat_buf);

// write arbitrary length to arbitrary addr
void cache_write_n(CacheTable *table, uint64_t addr, void *dat_buf, uint64_t s);

// insert one line at tag (start addr of a cache line)
void cache_insert(CacheTable *table, uint64_t tag, void *dat_buf);

// write arbitrary length to remote arbitrary addr
// obsence and clean in local
void remote_write_n(CacheTable *table, uint64_t addr, void *dat_buf, uint64_t s);

// prefetch arbitrary length from arbitrary remote addr
void prefetch(CacheTable *table, uint64_t addr, size_t l);

#ifdef __cplusplus
}
#endif
#endif
