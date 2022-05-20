#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "common.h"
#include "ambassador.h"
#include "prefetcher.h"

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

void pollAwait(uint64_t wr_id, Inflights *ins, BlockDLL *dll, Ambassador *a)
{
	// printAwaits(ins);
	// if no awaiting fetches
	if (ins->head == NULL || (wr_id > 0 && ins->head->b->wr_id > wr_id))
		return;

	AwaitBlock *tmp;
	while (ins->head != NULL && (wr_id == 0 || ins->head->b->wr_id <= wr_id))
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
			add_to_head(dll, b);
		}
		tmp = cur->next;
		free(cur);
		ins->head = tmp;
	}
  if (ins->head == NULL)
    ins->tail = NULL;
}

void printAwaits(Inflights *ins)
{
  AwaitBlock *cur = ins->head;
  printf("Awaits: ");
  while (cur != NULL)
  {
    printf("%" PRIu64 " ", cur->b->tag);
    cur = cur->next;
  }
  printf("\nTail: ");
  cur = ins->tail;
  while (cur != NULL)
  {
    printf("%" PRIu64 " ", cur->b->tag);
    cur = cur->next;
  }
  printf("\n");
}