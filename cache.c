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
	d->tail = d->head = NULL;
	return d;
}

uint64_t popVictim(BlockDLL *dll, CacheTable *table)
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
	victim->present = 0;
	if (victim->dirty) {
		// printf("Dirty ! Need flush\n");
		update_sync(table->amba->line_pool + victim->rbuf_offset, victim->tag, table->amba);
		victim->dirty = 0;
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

void addCold(BlockDLL *dll, Block *b)
{
	b->prev = dll->tail;
	if (!dll->tail)
		dll->head =dll->tail = b;
	else 
	{
		dll->tail->next = b;
		dll->tail = b;
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

int claim(FreeQueue *fq, uint64_t *offset)
{
	if (noFree(fq)) return 0;
	uint64_t room = fq->slots[fq->front];
	fq->front = (fq->front + 1) % fq->capacity;
	fq->frees -= 1;
	*offset = room;
	return 1;
}

void reclaim(FreeQueue *fq, uint64_t *offset)
{
	// will never full
	if (allFree(fq)) return;
	fq->end = (fq->end + 1) % fq->capacity;
	fq->slots[fq->end] = *offset;
	fq->frees += 1;
}

Ambassador *newAmbassador(uint32_t ring_size, uint64_t cls, void *req_buffer, void *recv_buffer)
{
	Ambassador *a = (Ambassador *)malloc(sizeof(Ambassador));
	a->ring_size = ring_size;
	a->cache_line_size = cls;
	a->req_size = sizeof(struct req) + sizeof(char) * cls;
	a->snid = (uint32_t *) malloc(sizeof(uint32_t) * ring_size);
	for (uint32_t i = 0; i < ring_size; ++i)
	{
		a->snid[i] = i;
	}
	a->front = 0;
	a->frees = a->ring_size;
	a->end = ring_size - 1;
	a->reqs = (char *) req_buffer;
	a->line_pool = (char *) recv_buffer;
	return a;
}

uint32_t get_sid(Ambassador *a)
{
	if (!a->frees)
	{
		printf("No free send buffer slot\n");
		// TODO: return wait signal
		exit(1);
	}
	uint32_t sid = a->snid[a->front];
	a->front = (a->front + 1) % a->ring_size;
	a->frees -= 1;
	return sid;
}

void ret_sid(Ambassador *a, uint32_t sid)
{
	if (a->frees == a->ring_size) return;
	a->end = (a->end + 1) % a->ring_size;
	a->snid[a->end] = sid;
	a->frees += 1;
}

void fetch_sync(Block *b, Ambassador *a)
{
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = b->tag;
	r->size = a->cache_line_size;
	r->type = 1;
	send(r, a->req_size);
	recv(a->line_pool + b->rbuf_offset, a->cache_line_size);
	ret_sid(a, send_buf_nid);
}

void update_sync(void *dat_buf, uint64_t tag, Ambassador *a)
{
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = tag;
	r->size = a->cache_line_size;
	r->type = 0;
	memcpy(r+1, dat_buf, sizeof(char) * a->cache_line_size);
	// send(r, sizeof(struct req));
	// send(r+1, a->cache_line_size);
	send(r, a->req_size);
	ret_sid(a, send_buf_nid);
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

	cache->amba = newAmbassador(128, cache_line_size, req_buffer, recv_buffer);

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
	printf("line: \n");
	for (size_t i = 0; i < cache_line_size * sizeof(char) / sizeof(uint64_t); ++ i)
	{
		printf("%" PRIu64 " ", *(vline + i));
	}
	printf("end-----\n");
}

void dllPrint(Block *head)
{
	Block *cur = head;
	printf("DLL: ");
	while (cur)
	{
		printf("%" PRIu64 " ", cur->tag);
		cur = cur->next;
	}
	printf("\n");
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
	uint64_t line_offset /* bytes */ = (addr & table->tag_mask);
	// printf("Access addr, tag, offset: %"PRIu64" %" PRIu64 " %" PRIu64 "\n", addr, tag, line_offset);
	// hashPrint(table->map);/
	HashStruct *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	// if not in cache, fetch
	if (tgt == NULL || !tgt->bptr->present)
	{
		table->misses += 1;
		// claim a space for new fetched data
		uint64_t rbuf_offset;
		if (!claim(table->fq, &rbuf_offset))
		{
			// rbuf filled up, need eviction
			rbuf_offset = popVictim(table->dll, table);
			// printf("Evict and found roffset: %" PRIu64 "\n", rbuf_offset);
		}
		if (tgt == NULL)
		{
			tgt = (HashStruct *) malloc(sizeof *tgt);
			// create new block, not present yet
			tgt->bptr = newBlock(rbuf_offset, tag, 0, 0);
			tgt->tag = (int) tag;
			// inser to map as hot 
			addHot(table->dll, tgt->bptr);
			HASH_ADD_INT(table->map, tag, tgt);
		}
		else
		{
			tgt->bptr->rbuf_offset = rbuf_offset;
			tgt->bptr->dirty = 0;
			// touch -> hot
			addHot(table->dll, tgt->bptr);
		}
		fetch_sync(tgt->bptr, table->amba);
		tgt->bptr->present = 1;
		// printf("Fetched ");
		// linePrint(table->amba->line_pool + rbuf_offset, table->cache_line_size);
		return table->amba->line_pool + rbuf_offset + line_offset;
	} 
	// if find target, touch and return
	touch(table->dll, tgt->bptr);

	// printf("Found ");
	// linePrint(table->amba->line_pool + tgt->bptr->rbuf_offset, table->cache_line_size);
	return table->amba->line_pool + tgt->bptr->rbuf_offset + line_offset;
}

// Write at location addr, size 8B
void cache_write(CacheTable *table, uint64_t addr, void *dat_buf)
{
	uint64_t tag = (addr & table->addr_mask) >> table->tag_shifts;
	uint64_t line_offset /* bytes */ = (addr & table->tag_mask);

	HashStruct *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	// if is a new cache entry or evicted line
	if (tgt == NULL || !tgt->bptr->present)
	{
		// claim room on rbuf for this cache line
		uint64_t rbuf_offset;
		if (!claim(table->fq, &rbuf_offset))
		{
			rbuf_offset = popVictim(table->dll, table);
		}
		// new to remote
		// write-back
		if (tgt == NULL)
		{
			memset(table->amba->line_pool + rbuf_offset, 0, sizeof(char) * table->cache_line_size);
			tgt = (HashStruct *) malloc(sizeof(HashStruct));
			tgt->bptr = newBlock(rbuf_offset, tag, 0, /* dirty */ 1);
			tgt->tag = (int) tag;
			// insert as hot
			HASH_ADD_INT(table->map, tag, tgt);
			addHot(table->dll, tgt->bptr);
		}
		else
		{
			// printf("Write to evicted, tag: %" PRIu64"\n", tgt->bptr->tag);
			// remote has stale version, pull and write-back
			tgt->bptr->rbuf_offset = rbuf_offset;
			fetch_sync(tgt->bptr, table->amba);
			tgt->bptr->dirty = 1;
			addHot(table->dll, tgt->bptr);
		}
		// write to rbuf
		memcpy(table->amba->line_pool + rbuf_offset + line_offset, dat_buf, sizeof(uint64_t));
		tgt->bptr->present = 1;
		return;
	}
	memcpy(table->amba->line_pool + tgt->bptr->rbuf_offset + line_offset, dat_buf, sizeof(uint64_t));
	tgt->bptr->dirty = 1;
}

// write one line into cache
void cache_insert(CacheTable *table, uint64_t tag, void *dat_buf)
{
	HashStruct *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	if (tgt == NULL || !tgt->bptr->present)
	{
		// claim room for line
		uint64_t rbuf_offset;
		if (!claim(table->fq, &rbuf_offset))
		{
			rbuf_offset = popVictim(table->dll, table);
		}
		if (tgt == NULL)
		{
			tgt = (HashStruct *) malloc(sizeof(HashStruct));
			tgt->bptr = newBlock(rbuf_offset, tag, 0, /* dirty */ 1);
			tgt->tag = (int) tag;
			// insert as hot
			HASH_ADD_INT(table->map, tag, tgt);
			addHot(table->dll, tgt->bptr);
		}
		else
		{
			// remote contains stale version
			// no need to pull
			tgt->bptr->rbuf_offset = rbuf_offset;
			tgt->bptr->dirty = 1;
			addHot(table->dll, tgt->bptr);
		}
		// write to rbuf
		memcpy(table->amba->line_pool + rbuf_offset, dat_buf, table->cache_line_size);
		tgt->bptr->present = 1;
		return;
	}
	memcpy(table->amba->line_pool + tgt->bptr->rbuf_offset, dat_buf, table->cache_line_size);
	tgt->bptr->dirty = 1;
}

void remote_write(CacheTable *table, uint64_t tag, void *dat_buf)
{
	update_sync(dat_buf, tag, table->amba);
	// create dummy block entry in map
	HashStruct *tgt = (HashStruct *) malloc(sizeof (HashStruct));
	tgt->bptr = newBlock(-1, tag, 0, 0);
	tgt->tag = (int) tag;
	HASH_ADD_INT(table->map, tag, tgt);
}