#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#include "cache_internal.h"
#include "common.h"
#include "cache.h"
#include "greeting.h"
#include "uthash.h"

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
