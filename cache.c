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
	uint64_t offset = victim->offset;
	free(victim);
	return offset;
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
	cache->misses = 0;

	cache->reqs = req_buffer;
	cache->line_pool = (char*) recv_buffer;

	uint64_t tag_possibles = ((uint64_t)-1) >> lbts;
	cache->map = NULL;
	cache->dll = initBlockDLL();
	cache->fq = initQueue(cache->max_size, cache->cache_line_size);
	return cache;
}

void hashPrint(HashStruct *hs)
{
	HashStruct *cur;
	for (cur = hs; cur != NULL && cur->bptr != NULL; cur = cur->hh.next) {
			printf("tag %d, offset %" PRIu64 "\n" , cur->tag, cur->bptr->offset);
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

// addr (bytes) -> offset with regard to *rbuf
char *cache_access(CacheTable *table, uint64_t addr)
{
	uint64_t tag = (addr & table->addr_mask) >> table->tag_shifts;
	uint64_t offset = (addr & table->tag_mask) / sizeof(char);
	// printf("Access addr, tag, offset: %"PRIu64" %" PRIu64 " %" PRIu64 "\n", addr, tag, offset);
	// hashPrint(table->map);
	HashStruct *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	// if not in cache, fetch
	if (tgt == NULL || tgt->bptr == NULL)
	{
		table->misses += 1;
		struct req *r = (struct req*) table->reqs;
		r->addr = tag;
		r->size = table->cache_line_size;
		r->type = 1;

		// claim a space for hot data
		// evict in claim
		uint64_t rbuf_offset = claim(table->fq, table->dll);
		// printf("Send tag: %"PRIu64"\n", r->addr);
    send(r, sizeof(struct req));
		recv(table->line_pool + rbuf_offset, table->cache_line_size);
		// linePrint(table->line_pool + rbuf_offset, table->cache_line_size);

		if (tgt == NULL) 
			tgt = (HashStruct *) malloc(sizeof *tgt);
		// create new block - dirty = 1
		tgt->bptr = newBlock(1, rbuf_offset);
		tgt->tag = (int) tag;
		// inser to map as hot 
		addHot(table->dll, tgt->bptr);
		HASH_ADD_INT(table->map, tag, tgt);

		return table->line_pool + rbuf_offset + offset;
	} 

	// get offset
	char *mem = table->line_pool + tgt->bptr->offset + offset;
	touch(table->dll, tgt->bptr);
	return mem;
}