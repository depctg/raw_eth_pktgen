#ifndef __CACHEINT__
#define __CACHEINT__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "cache.h"
#include "greeting.h"
#include "uthash.h"

Block *newBlock(uint64_t offset, uint64_t tag, uint8_t present, uint8_t dirty, uint64_t wr_id, uint32_t sid)
{
	Block *b = (Block *)malloc(sizeof(Block));
	b->rbuf_offset = offset;
	b->tag = tag;
	b->prev = b->next = NULL;
	b->present = present;
	b->dirty = dirty;
	b->wr_id = wr_id;
	b->sid = sid;
	return b;
}

BlockDLL *initBlockDLL()
{
	BlockDLL *d = (BlockDLL *)malloc(sizeof(BlockDLL));
	d->tail = d->head = NULL;
	return d;
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
		update_sync(table->amba->line_pool + victim->rbuf_offset, (victim->tag << table->tag_shifts), table->cache_line_size, table->amba);
		victim->dirty = 0;
	}
	// printf("Evict: %" PRIu64 ", get %" PRIu64 "\n", victim->tag, victim->rbuf_offset);
	victim->prev = NULL;
	victim->next = NULL;
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

void protect(BlockDLL *dll, Block *b)
{
	if (!dll->head || !b)
		return;
	if (dll->head == b)
		dll->head = b->next;
	if (b->next)
		b->next->prev = b->prev;
	if (b->prev)
		b->prev->next = b->next;
	return;
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

void fetch_sync(uint64_t addr, uint64_t rbuf_offset, Ambassador *a)
{
	// printf("Fetch addr: %" PRIu64 "\n", addr);
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = addr;
	r->size = a->cache_line_size;
	r->type = 1;
	send(r, a->req_size);
	recv(a->line_pool + rbuf_offset, a->cache_line_size);
	ret_sid(a, send_buf_nid);
}

void update_sync(void *dat_buf, uint64_t addr, uint64_t size, Ambassador *a)
{
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = addr;
	r->size = size;
	r->type = 0;
	memcpy(r+1, dat_buf, size);
	// send(r, sizeof(struct req));
	// send(r+1, a->cache_line_size);
	send(r, a->req_size);
	ret_sid(a, send_buf_nid);
}

uint64_t fetch_async(uint64_t addr, uint64_t rbuf_offset, Ambassador *a, uint32_t *sid)
{
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = addr;
	r->size = a->cache_line_size;
	r->type = 1;
	send_async(r, sizeof(struct req));
	return recv_async(a->line_pool + rbuf_offset, a->cache_line_size);
}

Inflights *initAwaits()
{
	Inflights *ins = (Inflights *) malloc(sizeof(Inflights));
	ins->head = ins->tail = NULL;
	return ins;
}

void awaitFetch(Inflights *ins, Block *b)
{
	AwaitBlock *ab = (AwaitBlock *) malloc(sizeof(AwaitBlock));
	ab->b = b;
	ab->next = NULL;

	if (ins->tail == NULL)
	{
		ins->head = ins->tail = ab;
		return;
	}
	ins->tail->next = ab;
	ins->tail = ab;
}

void pollAwait(Inflights *ins, BlockDLL *dll, Ambassador *a)
{
	// if no awaiting fetches
	if (ins->head == NULL)
		return;

	AwaitBlock *tmp;
	while (ins->head != NULL)
	{
		AwaitBlock *cur = ins->head;
		Block *b = cur->b;
		if (b->wr_id != -1 && b->sid != -1)
		{
			// polling prefetch
			// printf("Polling tag: %" PRIu64 "\n", b->tag);
			poll(b->wr_id);
			ret_sid(a, b->sid);
			b->dirty = 0;
			b->wr_id = -1;
			b->sid = -1;
			addHot(dll, b);
		}
		tmp = cur->next;
		free(cur);
		ins->head = tmp;
	}
	ins->tail = NULL;
}

#ifdef __cplusplus
}
#endif
#endif