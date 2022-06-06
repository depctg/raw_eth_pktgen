#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "uthash.h"
#include "mem_block.h"

Block *newBlock(uint64_t rbuf_offset, uint64_t tag, enum BLOCK_STATUS bs, uint8_t dirty, uint64_t wr_id, uint16_t weight)
{
	Block *b = (Block *)malloc(sizeof(Block));
	b->rbuf_offset = rbuf_offset;
	b->tag = tag;
	b->prev = b->next = NULL;
	b->wr_id = wr_id;
	b->status = bs;
	b->dirty = dirty;
  b->weight = weight;
	return b;
}

HashBlock *new_entry(uint64_t tag, Block *bptr)
{
  HashBlock *e = (HashBlock *) malloc(sizeof(HashBlock));
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

void add_to_head(BlockDLL *dll, Block *b)
{
	b->next = dll->head;
	// dllPrint(dll);
	if (!dll->tail) {
		dll->head = dll->tail = b;
	}
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

void remove_from_dll(BlockDLL *dll, Block *b)
{
	if (!dll->head || !b)
		return;
	if (dll->head == b)
		dll->head = b->next;
  if (dll->tail == b)
    dll->tail = b->prev;
	if (b->next)
		b->next->prev = b->prev;
	if (b->prev)
		b->prev->next = b->next;
	b->prev = b->next = NULL;
	return;
}

Block *pop_last(BlockDLL *dll) {
  if (!dll->tail) 
    return NULL;
  if (dll->head == dll->tail)
    dll->head = NULL;
  
  Block *vic = dll->tail;
  dll->tail = dll->tail->prev;
  if (dll->tail)
    dll->tail->next = NULL;
  vic->prev = vic->next = NULL;
  return vic;
}

void dllPrint(BlockDLL *dll)
{
	Block *cur = dll->head;
	printf("DLL: head ->");
	while (cur)
	{
		printf("%" PRIu64 "-%d ", cur->tag, cur->weight);
		cur = cur->next;
	}
  if (dll->tail)
    printf(" | tail %" PRIu64 "-%d", dll->tail->tag, dll->tail->weight);
	printf("\n");
}

void destruct_dll(BlockDLL *dll) {
	Block *cur = dll->head;
	while (cur) {
		Block *tmp = cur->next;
		free(cur);
		cur = tmp;
	}
	free(dll);
}