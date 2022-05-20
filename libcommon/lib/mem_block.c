#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "uthash.h"
#include "mem_block.h"

Block *newBlock(uint64_t offset, uint64_t tag, uint8_t present, uint8_t dirty, uint64_t wr_id, uint32_t sid, uint16_t weight)
{
	Block *b = (Block *)malloc(sizeof(Block));
	b->rbuf_offset = offset;
	b->tag = tag;
	b->prev = b->next = NULL;
	b->present = present;
	b->dirty = dirty;
	b->wr_id = wr_id;
	b->sid = sid;
  b->weight = weight;
	return b;
}

HashStruct *new_entry(uint64_t tag, Block *bptr)
{
  HashStruct *e = (HashStruct *) malloc(sizeof(HashStruct));
  e->bptr = bptr;
  e->tag = (int) tag;
  return e;
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

void add_to_head(BlockDLL *dll, Block *b)
{
	b->next = dll->head;
	if (!dll->tail)
		dll->head = dll->tail = b;
	else {
		dll->head->prev = b;
		dll->head = b;
	}
}

void add_to_tail(BlockDLL *dll, Block *b)
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