#ifndef __LRUCACHE__
#define __LRUCACHE__
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "common.h"
#include "mem_block.h"
#include "mem_slicer.h"
#include "replacement.h"
#include "ambassador.h"

#ifdef __cplusplus
extern "C"
{
#endif

enum {
    CACHE_WRITE = 0,
    CACHE_READ = 1,
};

typedef struct CacheTable
{
	uint64_t tag_mask;
	uint64_t addr_mask;
	uint64_t max_size;
	uint64_t misses;
	uint64_t accesses;

	Ambassador *amba;

	uint64_t cache_line_size;
	uint8_t tag_shifts;

	// a hashmap of blocks
	HashBlock *map;
	// Replacement Policy
	Policy *rplc;
	// a queue of free slots
	FreeQueue *fq;
} CacheTable;

// max_size, cache_line_size (byte)
CacheTable *createCacheTable(
	uint64_t max_size,
	uint64_t cache_line_size,
	void *req_buffer,
	void *recv_buffer,
	int max_weight,
	void (*fresh_add)(struct Policy *, Block *b),
	void (*access_existing)(struct Policy *, Block *b),
	Block *(*pop_victim)(struct Policy *)
);

uint64_t pop_for_rbuf(CacheTable *table);

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


// prefetch arbitrary cache line
// pending cache lines will never be evicted
// and will be evictable after accessed once

// the overlapped pre-fetch will be cancelled
// if is locally available, no pf will be performed
// TODO: use another thread to deal with polling
void prefetch(CacheTable *table, uint64_t addr /* start address of a cache line */);

#ifdef __cplusplus
}
#endif
#endif
