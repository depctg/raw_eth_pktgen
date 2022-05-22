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

void remove_from_dll(BlockDLL *dll, Block *b)
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

Victim *initVictim(int range) {
	Victim *v = (Victim *) malloc(sizeof(Victim));
	v->weight_dll = (BlockDLL **) malloc(sizeof(BlockDLL *) * range);
	for (int i = 0; i < range; ++i) {
		v->weight_dll[i] = initBlockDLL();
	}
	return v;
}

void add_to_victim(Victim *v, Block *b) {
	BlockDLL *dll = v->weight_dll[b->weight];
	add_to_head(dll, b);
}

void remove_from_victim(Victim *v, Block *b) {
	BlockDLL *dll_old = v->weight_dll[b->weight];
	remove_from_dll(dll_old, b);
}