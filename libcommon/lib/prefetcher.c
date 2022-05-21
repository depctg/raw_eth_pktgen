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