#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "cache.h"
#include "greeting.h"
#include <inttypes.h>
#include "uthash.h"


Block *newBlock(uint64_t offset, uint64_t tag, uint8_t present, uint8_t dirty)
{
	Block *b = (Block *)malloc(sizeof(Block));
	b->rbuf_offset = offset;
	b->tag = tag;
	b->prev = b->next = NULL;
	b->present = present;
	b->dirty = dirty;
	return b;
}

BlockDLL *initBlockDLL()
{
	BlockDLL *d = (BlockDLL *)malloc(sizeof(BlockDLL));
	d->head = d->tail = NULL;
	return d;
}

uint64_t popVictim(BlockDLL *dll, HashStruct *map)
{
	if (!dll->tail) {
		fprintf(stderr, "Eviction of empty cache pool\n");
		exit(1);
	}

	if (dll->head == dll->tail)
		dll->head = NULL;

	Block *victim = dll->tail;
	dll->tail = dll->tail->prev;
	victim->present = 0;
	if (victim->dirty) {
		printf("Dirty ! Need flush\n");
	}
	return victim->rbuf_offset;
}

void addHot(BlockDLL *dll, Block *b)
{
	b->next = dll->head;
	if (!dll->tail)
		dll->head = dll->tail = b;
	else {
		dll->head->prev = b;
		dll->head = b;
	}
}

void touch(BlockDLL *dll, Block *b)
{
	// if not most hot
	if (dll->head != b)
	{
		b->prev->next = b->next;
		if (b->next)
			b->next->prev = b->prev;
		if (b == dll->tail)
		{
			dll->tail = b->prev;
			dll->tail->next = NULL;
		}
		b->next = dll->head;
		b->prev = NULL;
		b->next->prev = b;
		dll->head = b;
	}
}

FreeQueue* initQueue(uint64_t max_size, uint32_t cache_line_size)
{
	FreeQueue *fq = (FreeQueue *) malloc(sizeof(FreeQueue));
	fq->capacity = max_size / cache_line_size;

	fq->slots = (uint64_t *) malloc(sizeof(uint64_t) * fq->capacity);
	for (size_t i = 0; i < fq->capacity; i++)
	{
		fq->slots[i] = i * cache_line_size;
	}

	fq->front = 0;
	fq->frees = fq->capacity;
	fq->end = fq->capacity - 1;
	return fq;
}

int allFree(FreeQueue *fq)
{
	return fq->frees == fq->capacity;
}

int noFree(FreeQueue *fq)
{
	return fq->frees == 0;
}

uint64_t claim(FreeQueue *fq, BlockDLL *dll, HashStruct *map)
{
	if (noFree(fq)) return popVictim(dll, map);
	uint64_t room = fq->slots[fq->front];
	fq->front = (fq->front + 1) % fq->capacity;
	fq->frees -= 1;
	return room;
}

void reclaim(FreeQueue *fq, uint64_t s)
{
	// will never full
	if (allFree(fq)) return;
	fq->end = (fq->end + 1) % fq->capacity;
	fq->slots[fq->end] = s;
	fq->frees += 1;
}

CacheTable *createCacheTable(
	uint64_t max_size,
	uint32_t cache_line_size,
	void *req_buffer,
	void *recv_buffer)
{
	CacheTable *cache = (CacheTable *)malloc(sizeof(CacheTable));
	uint8_t lbts = log2(cache_line_size * sizeof(char));
	cache->tag_shifts = lbts;
	cache->tag_mask = ((uint64_t) 1 << lbts) - 1;
	cache->addr_mask = ~(cache->tag_mask);
	cache->max_size = max_size;
	cache->cache_line_size = cache_line_size;
	cache->accesses = 0;
	cache->misses = 0;

	cache->reqs = req_buffer;
	cache->line_pool = (char*) recv_buffer;

	cache->map = NULL;
	cache->dll = initBlockDLL();
	cache->fq = initQueue(cache->max_size, cache->cache_line_size);
	return cache;
}

void hashPrint(HashStruct *hs)
{
	HashStruct *cur;
	for (cur = hs; cur != NULL && cur->bptr != NULL; cur = cur->hh.next) {
			printf("tag %d, offset %" PRIu64 "\n" , cur->tag, cur->bptr->rbuf_offset);
	}
}

void linePrint(char *line, uint32_t cache_line_size)
{
	uint64_t *vline = (uint64_t *) line;
	printf("Received line: \n");
	for (size_t i = 0; i < cache_line_size * sizeof(char) / sizeof(uint64_t); ++ i)
	{
		printf("%" PRIu64 "\n", *(vline + i));
	}
}

// Access virtual address {addr}
// addr:
// |  tag (64-L)b | LOFST (L)b  |
// LOFST: line_offset

// Cache table reserves rbuf memory within [rbuf', rbuf' + max_size]
// rbuf' = rbuf + N

// Find ROFST: rbuf_offset in the cache table
// ROFST = map[tag]

// return the correct address
// {rbuf'}      {rbuf' + ROFST}         														 {rbuf' + max_size}
//  |       ...        | < ------- cache_line_size  -------> |   ...        |
//										 | <- LOFST -> |
//																   return {rbuf' + ROFST + LOFST}

char *cache_access(CacheTable *table, uint64_t addr)
{
	table->accesses += 1;
	uint64_t tag = (addr & table->addr_mask) >> table->tag_shifts;
	uint64_t line_offset /* bytes */ = (addr & table->tag_mask) / sizeof(char);
	// printf("Access addr, tag, offset: %"PRIu64" %" PRIu64 " %" PRIu64 "\n", addr, tag, offset);
	// hashPrint(table->map);
	HashStruct *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	// if not in cache, fetch
	if (tgt == NULL || !tgt->bptr->present)
	{
		table->misses += 1;
		struct req *r = (struct req*) table->reqs;
		r->addr = tag;
		r->size = table->cache_line_size;
		r->type = 1;

		// TODO: async
    send(r, sizeof(struct req));

		// claim a space for hot data, evict if needed
		uint64_t rbuf_offset = claim(table->fq, table->dll, table->map);

		// printf("Send tag: %"PRIu64"\n", r->addr);
		recv(table->line_pool + rbuf_offset, table->cache_line_size);
		// linePrint(table->line_pool + rbuf_offset, table->cache_line_size);

		if (tgt == NULL)
		{
			tgt = (HashStruct *) malloc(sizeof *tgt);
			// create new block
			tgt->bptr = newBlock(rbuf_offset, tag, 1, 0);
			tgt->tag = (int) tag;
			// inser to map as hot 
			addHot(table->dll, tgt->bptr);
			HASH_ADD_INT(table->map, tag, tgt);
		}
		else
		{
			tgt->bptr->rbuf_offset = rbuf_offset;
			tgt->bptr->present = 1;
			// if is pulled, the dirty flag is clean
			// touch -> hot
			touch(table->dll, tgt->bptr);
		}
		return table->line_pool + rbuf_offset + line_offset;
	} 

	// if find target, touch and return
	touch(table->dll, tgt->bptr);
	return table->line_pool + tgt->bptr->rbuf_offset + line_offset;
}