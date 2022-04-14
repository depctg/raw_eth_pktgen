#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "cache.h"
#include "greeting.h"


Block *newBlock(uint8_t dirty, uint64_t offset)
{
	Block *b = (Block *)malloc(sizeof(Block));
	b->dirty = dirty;
	b->offset = offset;
	b->prev = b->next = NULL;
	return b;
}

BlockDLL *initBlockDLL()
{
	BlockDLL *d = (BlockDLL *)malloc(sizeof(BlockDLL));
	d->head = d->tail = NULL;
	return d;
}

uint64_t popVictim(BlockDLL *dll)
{
	if (!dll->tail) {
		fprintf(stderr, "Eviction of empty cache pool\n");
		exit(1);
	}

	if (dll->head == dll->tail)
		dll->head = NULL;

	Block *victim = dll->tail;
	dll->tail = dll->tail->prev;
	if (dll->tail)
		dll->tail->next = NULL;
	return victim->offset;
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

FreeQueue* initQueue(uint64_t max_size, uint64_t cache_line_size)
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

uint64_t claim(FreeQueue *fq, BlockDLL *dll)
{
	if (noFree(fq)) return popVictim(dll);
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
	uint64_t cache_line_size,
	void *req_buffer,
	void *recv_buffer)
{
	CacheTable *cache = (CacheTable *)malloc(sizeof(CacheTable));
	uint8_t lbts = log2(cache_line_size);
	cache->tag_mask = ((uint64_t) 1 << lbts) - 1;
	cache->addr_mask = ~(cache->tag_mask);
	cache->max_size = max_size;
	cache->cache_line_size = cache_line_size;

	cache->reqs = req_buffer;
	cache->line_pool = (char*) recv_buffer;

	cache->map = (Block **)malloc(max_size * sizeof(Block *));
	for (size_t i = 0; i < max_size; ++i)
		cache->map[i] = NULL;
	
	cache->dll = initBlockDLL();
	cache->fq = initQueue(max_size, cache_line_size);
	return cache;
}

// void insert(uint64_t addr)
// {

// }

char *cache_access(CacheTable *table, uint64_t addr)
{
	uint64_t tag = addr & table->addr_mask;
	uint64_t offset = addr & table->tag_mask;
	Block *tgt = table->map[tag];
	// if not in cache, fetch
	if (tgt == NULL)
	{
		struct req *r = (struct req*) table->reqs;
		r->addr = tag;
		r->size = table->cache_line_size;
		r->type = 1;
    send(r, sizeof(struct req));
		return NULL;
	}

	// get offset
	char *mem = /*(char *)*/ table->line_pool + tgt->offset;
	touch(table->dll, tgt);
	return mem;
}