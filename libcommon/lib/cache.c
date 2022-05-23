#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#include "cache_internal.h"
#include "common.h"
#include "mem_block.h"
#include "mem_slicer.h"
#include "ambassador.h"
#include "cache.h"
#include "greeting.h"
#include "uthash.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

static inline uint64_t _async_request_sge(
        Block *b, Ambassador *a, uint64_t tag, int type
    ) {
	uint32_t sid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + sid * a->req_size);
	r->size = a->cache_line_size;
	r->addr = type == CACHE_READ ? b->tag : tag;
	r->type = type;

    char * cacheline = a->line_pool + b->rbuf_offset;

    // build sge RDMA packet

    if (type == CACHE_READ) {
        send_async(r, sizeof(struct req));
        return recv_async(cacheline, a->cache_line_size);
    } else if (type == CACHE_WRITE) {
        size_t msgsize = a->cache_line_size + sizeof(struct req);
        struct ibv_sge sge[2];

        // request
        sge[0].addr = (uintptr_t)r;
        sge[0].length = sizeof(struct req);
        sge[0].lkey = rmr->lkey;
        // data
        sge[1].addr = (uintptr_t)cacheline;
        sge[1].length = a->cache_line_size;
        sge[1].lkey = rmr->lkey;

        return send_async_sge(sge, 2);
    }
}


CacheTable *createCacheTable(
	uint64_t max_size,
	uint64_t cache_line_size,
	void *req_buffer,
	void *recv_buffer,
	int max_weight,
	void (*fresh_add)(struct Policy *, struct Block *b),
	void (*access_existing)(struct Policy *, struct Block *b),
	struct Block *(*pop_victim)(struct Policy *))
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

	cache->amba = newAmbassador(64, cache_line_size, req_buffer, recv_buffer);

	cache->map = NULL;
	cache->rplc = initReplacement(max_weight, fresh_add, access_existing, pop_victim);
	cache->fq = initQueue(cache->max_size, cache->cache_line_size);
	return cache;
}

void hashPrint(HashBlock *hs)
{
	HashBlock *cur;
	for (cur = hs; cur != NULL && cur->bptr != NULL; cur = cur->hh.next) {
			printf("tag %d, status %d\n" , cur->tag, cur->bptr->status);
	}
}

void linePrint(char *line, uint64_t cache_line_size)
{
	uint64_t *vline = (uint64_t *) line;
	printf("line: \n");
	for (size_t i = 0; i < cache_line_size * sizeof(char) / sizeof(uint64_t); ++ i)
	{
		printf("%" PRIu64 " ", *(vline + i));
	}
	printf("end-----\n");
}

uint64_t pop_for_rbuf(CacheTable *table)
{
	// make sure no wr is holding space
	cq_consumer(0, RECV, table->amba, table->rplc);
	Block *victim = table->rplc->pop_victim(table->rplc);
	if (victim == NULL) {
		// victim list is empty but need eviction
		fprintf(stderr, "Eviction of empty cache pool\n");
		exit(1);
	}
	victim->status = absent;
	if (victim->dirty) {
		update_sync(table->amba->line_pool + victim->rbuf_offset, (victim->tag << table->tag_shifts), table->cache_line_size, table->amba, table->rplc);
		victim->dirty = 0;
	}
	return victim->rbuf_offset;
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
	uint64_t tag = addr >> table->tag_shifts;
	uint64_t line_offset /* bytes */ = (addr & table->tag_mask);
	// printf("Access addr, tag, offset: %"PRIu64" %" PRIu64 " %" PRIu64 "\n", addr, tag, line_offset);
	// hashPrint(table->map);
	HashBlock *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	// if not in cache, fetch
	if (tgt == NULL || tgt->bptr->status == absent)
	{
		table->misses += 1;
		// claim a space for new fetched data
		uint64_t rbuf_offset;
		if (!claim(table->fq, &rbuf_offset))
		{
			// rbuf filled up, need eviction
			rbuf_offset = pop_for_rbuf(table);
		}
		// printf("Ac rbuf ofst: %" PRIu64 "\n", rbuf_offset);
		if (tgt == NULL)
		{
			tgt = (HashBlock *) malloc(sizeof *tgt);
			// create new block, not present yet
			tgt->bptr = newBlock(rbuf_offset, tag, pending, 0, -1, 0);
			tgt->tag = (int) tag;
			HASH_ADD_INT(table->map, tag, tgt);
		}
		else
		{
			tgt->bptr->rbuf_offset = rbuf_offset;
			tgt->bptr->status = pending;
			tgt->bptr->wr_id = -1;
			tgt->bptr->weight = 0;
			tgt->bptr->dirty = 0;
		}
		fetch_sync(tgt->bptr, table->amba, table->rplc, table->tag_shifts);
		// printf("Fetched ");
		// linePrint(table->amba->line_pool + rbuf_offset, table->cache_line_size);
		return table->amba->line_pool + rbuf_offset + line_offset;
	} 
	else if (tgt->bptr->status == pending)
	{
		// polling pending wr_id
		// printf("Access pending wr_id: %" PRIu64 "\n", tgt->bptr->wr_id);
		cq_consumer(tgt->bptr->wr_id, RECV, table->amba, table->rplc);
		// hashPrint(table->map);
	}
	else
		// if find target, touch and return
		table->rplc->access_existing(table->rplc, tgt->bptr);

	// printf("Found ");
	// linePrint(table->amba->line_pool + tgt->bptr->rbuf_offset, table->cache_line_size);
	return table->amba->line_pool + tgt->bptr->rbuf_offset + line_offset;
}

void write_to_CL(CacheTable *table, uint64_t tag, uint64_t line_offset, void *dat_buf, uint64_t s)
{
	// printf("tag %" PRIu64 ", lofst %" PRIu64 ", size %" PRIu64 "\n", tag, line_offset, s);
	HashBlock *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	// if is a new cache entry or evicted line
	if (tgt == NULL || tgt->bptr->status == absent)
	{
		// claim room on rbuf for this cache line
		uint64_t rbuf_offset;
		if (!claim(table->fq, &rbuf_offset)) 
			rbuf_offset = pop_for_rbuf(table);
		// new to remote
		// write-back
		if (tgt == NULL)
		{
			memset(table->amba->line_pool + rbuf_offset, 0, sizeof(char) * table->cache_line_size);
			tgt = (HashBlock *) malloc(sizeof(HashBlock));
			tgt->bptr = newBlock(rbuf_offset, tag, present, /* dirty */ 1, -1, 0);
			tgt->tag = (int) tag;
			// insert as hot
			HASH_ADD_INT(table->map, tag, tgt);
			table->rplc->fresh_add(table->rplc, tgt->bptr);
		}
		else
		{
			// printf("Write to evicted, tag: %" PRIu64"\n", tgt->bptr->tag);
			// remote has stale version, pull and write-back
			tgt->bptr->rbuf_offset = rbuf_offset;
			// fetch sync will add to victims
			fetch_sync(tgt->bptr, table->amba, table->rplc, table->tag_shifts);
			tgt->bptr->dirty = 1;
		}
		// write to rbuf
		memcpy(table->amba->line_pool + rbuf_offset + line_offset, dat_buf, s);
	}
	else if (tgt->bptr->status == pending)
	{
		// polling pending 
		cq_consumer(tgt->bptr->wr_id, RECV, table->amba, table->rplc);
		memcpy(table->amba->line_pool + tgt->bptr->rbuf_offset + line_offset, dat_buf, s);
		tgt->bptr->dirty = 1;
	}
	else
	{
		memcpy(table->amba->line_pool + tgt->bptr->rbuf_offset + line_offset, dat_buf, s);
		tgt->bptr->dirty = 1;
		table->rplc->access_existing(table->rplc, tgt->bptr);
	}
	return;
}

void cache_write(CacheTable *table, uint64_t addr, void *dat_buf)
{
	uint64_t tag = addr >> table->tag_shifts;
	uint64_t line_offset /* bytes */ = (addr & table->tag_mask);
	write_to_CL(table, tag, line_offset, dat_buf, sizeof(uint64_t));
}

// write one line into cache
void cache_insert(CacheTable *table, uint64_t tag, void *dat_buf)
{
	HashBlock *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	write_to_CL(table, tag, 0, dat_buf, table->cache_line_size);
}

void cache_write_n(CacheTable *table, uint64_t addr, void *dat_buf, uint64_t s)
{
	// first cache line tag of addr
	uint64_t ftag = addr >> table->tag_shifts;
	// last tag
	uint64_t ltag = (addr + s) >> table->tag_shifts;
	uint64_t written_l = 0;
	// printf("ftag %" PRIu64 ", ltag %" PRIu64 "\n", ftag, ltag);
	// write first cache line
	uint64_t line_offset = addr & table->tag_mask;
	uint64_t cur_length = min(s-written_l, table->cache_line_size - line_offset);
	write_to_CL(table, ftag, line_offset, (char *)dat_buf + written_l, cur_length);
	ftag ++;
	written_l += cur_length;

	// write rest lines
	while (ftag <= ltag)
	{
		// line offset = 0 for these lines
		uint64_t cur_length = min(s-written_l, table->cache_line_size);
		if (cur_length > 0)
			write_to_CL(table, ftag, 0, (char *)dat_buf + written_l, cur_length);
		ftag ++;
		written_l += cur_length;
	}
	return;
}

void _remote_write(CacheTable *table, uint64_t addr, uint64_t tag, uint64_t line_offset, void *dat_buf, uint64_t s)
{
	HashBlock *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	// if not accessed
	// create dummy block entry in map
	// policy will not have this entry
	// since this memory cannot be evicted
	if (tgt == NULL)
	{
		tgt = (HashBlock *) malloc(sizeof(HashBlock));
		tgt->bptr = newBlock(-1, tag, absent, 0, -1, 0);
		tgt->tag = (int) tag;
		HASH_ADD_INT(table->map, tag, tgt);
	} 
	else if (tgt->bptr->status == present)
	{
		// locally presented
		// merge locally and write to remote
		// will not change dirty status of local cache line
		memcpy(table->amba->line_pool + tgt->bptr->rbuf_offset + line_offset, dat_buf, s);
	}
	else if (tgt->bptr->status == pending)
	{
		cq_consumer(tgt->bptr->wr_id, RECV, table->amba, table->rplc);
		memcpy(table->amba->line_pool + tgt->bptr->rbuf_offset + line_offset, dat_buf, s);
	}
	update_sync(dat_buf, addr, s, table->amba, table->rplc);
}

void remote_write_n(CacheTable *table, uint64_t addr, void *dat_buf, uint64_t s)
{
	// first cache line tag of addr
	uint64_t ftag = (addr & table->addr_mask) >> table->tag_shifts;
	// last tag
	uint64_t ltag = ((addr + s) & table->addr_mask) >> table->tag_shifts;
	uint64_t written_l = 0;

	// write first segment
	uint64_t line_offset = addr & table->tag_mask;
	uint64_t cur_length = min(s - written_l, table->cache_line_size - line_offset);
	_remote_write(table, addr + written_l, ftag, line_offset, (char *) dat_buf + written_l, cur_length);
	ftag ++;
	written_l += cur_length;

	// write rest segments
	while (ftag <= ltag)
	{
		// line offset = 0 for these lines
		uint64_t cur_length = min(s - written_l, table->cache_line_size);
		if (cur_length > 0)
			_remote_write(table, addr + written_l, ftag, 0, (char *) dat_buf + written_l, cur_length);
		ftag ++;
		written_l += cur_length;
	}
	return;
}


void prefetch(CacheTable *table, uint64_t addr)
{
	uint64_t tag = addr >> table->tag_shifts;
	HashBlock *tgt;
	HASH_FIND_INT(table->map, &tag, tgt);
	// printf("Prefetch addr, tag: %" PRIu64 ", %" PRIu64 "\n", addr, (addr & table->addr_mask) >> table->tag_shifts);
	if (tgt == NULL || tgt->bptr->status == absent)
	{
		// claim room for this cache line
		uint64_t rbuf_offset;
		if (!claim(table->fq, &rbuf_offset)) 
			rbuf_offset = pop_for_rbuf(table);
		// prefetch
		// printf("Pf rbuf ofst: %" PRIu64 "\n", rbuf_offset);
		if (tgt == NULL)
		{
			tgt = (HashBlock *) malloc(sizeof(HashBlock));
			tgt->bptr = newBlock(rbuf_offset, tag, pending, 0, -1, 0);
			tgt->tag = (int) tag;
			HASH_ADD_INT(table->map, tag, tgt);
		}
		else
		{
			// if not at local
			tgt->bptr->rbuf_offset = rbuf_offset;
			tgt->bptr->status = pending;
			tgt->bptr->wr_id = -1;
		}
		uint64_t wr_id = fetch_async(tgt->bptr, table->amba, table->rplc, table->tag_shifts);
		// awaitFetch(table->ins, tgt->bptr);
		// printf("Prefetch tag: %" PRIu64 ", wr_id%" PRIu64 "\n", tag, wr_id);
	}
	else if (tgt->bptr->status == pending)
	{
		cq_consumer(tgt->bptr->wr_id, RECV, table->amba, table->rplc);
	}
	else
	{
		// locally available but not overwrite
		table->rplc->access_existing(table->rplc, tgt->bptr);
	}
	// hashPrint(table->map);
}