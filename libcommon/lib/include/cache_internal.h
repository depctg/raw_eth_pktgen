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
#include "mem_block.h"
#include "mem_slicer.h"
#include "ambassador.h"

uint64_t popVictim(BlockDLL *dll, CacheTable *table)
{
	// first poll pending prefetch if any
	pollAwait(0, table->ins, dll, table->amba);
	if (!dll->tail) {
		// victim list is empty but need eviction
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

#ifdef __cplusplus
}
#endif
#endif