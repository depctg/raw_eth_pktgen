#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "uthash.h"
#include "mem_block.h"
#include "replacement.h"

Victim *initVictim(uint64_t w) {
	Victim *v = (Victim *) malloc(sizeof(Victim));
  v->max_weight = w;
	v->weight_dll = (BlockDLL **) malloc(sizeof(BlockDLL *) * (w+1));
	for (uint64_t i = 0; i <= w; ++i) {
		v->weight_dll[i] = initBlockDLL();
	}
	return v;
}

void add_to_victim(Victim *v, Block *b) {
	BlockDLL *dll = v->weight_dll[b->weight];
	add_to_head(dll, b);
}

void remove_from_victim(Victim *v, Block *b) {
	BlockDLL *dll = v->weight_dll[b->weight];
	remove_from_dll(dll, b);
}

void inspect(Victim *v) {
  for (uint64_t i = 0; i <= v->max_weight; ++ i) {
    if (v->weight_dll[i]->head != NULL) {
      printf("Weight %" PRIu64 " :\n", i);
      dllPrint(v->weight_dll[i]);
    }
  }
}

void destruct_victim(Victim *v) {
  for (uint64_t i = 0; i <= v->max_weight; ++i) {
    destruct_dll(v->weight_dll[i]);
  }
  free(v);
}

Policy *initReplacement(uint64_t max_weight, \
                        void (*fresh_add)(struct Policy *, Block *b), \
                        void (*access_existing)(struct Policy *, Block *b), \
                        Block *(*pop_victim)(struct Policy *)) {
  Policy *p = (Policy *) malloc(sizeof(Policy));
  p->max_weight = max_weight;
  p->v = initVictim(max_weight);

  p->future_refs = NULL;
  p->future_dist = NULL;
  p->precomp_evicts = NULL;
  p->future_depth = 0;
  p->time_step = 0;
  p->prebuff_step = 0;

  p->fresh_add = fresh_add;
  p->access_existing = access_existing;
  p->pop_victim = pop_victim;
  return p;
}

void add_to_LRU(Policy *p, Block *b) {
  if (b->weight != 0)
    b->weight = 0;
  add_to_victim(p->v, b);
}

void touch_LRU(Policy *p, Block *b) {
  if (b->weight != 0)
    b->weight = 0;
  touch(p->v->weight_dll[0], b);
}

Block *pop_LRU(Policy *p) {
  return pop_last(p->v->weight_dll[0]);
}

void add_to_LFU(Policy *p, Block *b) {
  b->weight = (b->weight < p->max_weight ? b->weight : p->max_weight);
  add_to_victim(p->v, b);
}

void touch_LFU(Policy *p, Block *b) {
  // dllPrint(p->v->weight_dll[b->weight]);
  remove_from_victim(p->v, b);
  // dllPrint(p->v->weight_dll[b->weight]);
  b->weight = (b->weight + 1 < p->max_weight ? b->weight + 1 : p->max_weight);
  add_to_victim(p->v, b);
}

Block *pop_LFU(Policy *p) {
  for (int i = 0; i <= p->max_weight; ++i) {
    Block *vic = pop_last(p->v->weight_dll[i]);
    if (vic != NULL) {
      // printf("evict: tag %" PRIu64 ", weight %d\n", vic->tag, vic->weight);
      return vic;
    }
  }
  return NULL;
}

/* args:
    refs_tag: future reference of cache line's tag 
    depth   : length of given future refs
    capacity: max number of cache lines 
    */
void regist_future(Policy *p, uint64_t *refs_tag, uint64_t depth, uint64_t capacity) {
  if (p->max_weight != depth) {
    p->max_weight = depth;
    destruct_victim(p->v);
    p->v = initVictim(depth);
  }
  p->future_depth = depth;
  p->future_refs = refs_tag;
  p->future_dist = (uint64_t *) malloc(sizeof(uint64_t) * depth);
  // a list of evictions, maximum length is depth - capacity
  // if all access are different - infinit reuse-dist
  p->precomp_evicts = (uint64_t *) malloc(sizeof(uint64_t) * (depth - capacity));

  // First Pass:
  // generate re-use distance map for each cache line
  HashFuture *hf = NULL;
  for (uint64_t t = depth - 1; t != -1; --t) {
    uint64_t tag = refs_tag[t];
    // printf("%" PRIu64 "\n", t);
    HashFuture *tgt;
    HASH_FIND_INT(hf, &tag, tgt);
    if (tgt == NULL) {
      // no future ref
      p->future_dist[t] = depth;
      tgt = (HashFuture *) malloc(sizeof(HashFuture));
      tgt->ref_tag = (int) tag;
      tgt->time_step = t;
      HASH_ADD_INT(hf, ref_tag, tgt);
    } else {
      // has future ref
      p->future_dist[t] = tgt->time_step;
      // update hash
      HASH_DEL(hf, tgt);
      tgt->time_step = t;
      HASH_ADD_INT(hf, ref_tag, tgt);
    }
  }
}

void add_to_OPT(Policy *p, Block *b) {
  if (p->prebuff_step >= p->future_depth) {
    printf("No future\n");
    exit(1);
  }
  while (p->future_refs[p->prebuff_step] != b->tag) {
    if (p->prebuff_step + 1 >= p->future_depth) {
      printf("No future\n");
      exit(1);
    }
    p->prebuff_step ++;
  }
  b->weight = p->future_dist[p->prebuff_step ++]; 
  add_to_victim(p->v, b);
}

void touch_OPT(Policy *p, Block *b) {
  if (p->future_refs[p->time_step] != b->tag) {
    printf("Not accurate touch, expect %" PRIu64 ", get %" PRIu64 "\n", p->future_refs[p->time_step], b->tag);
    exit(1);
  }
  if (b->weight != p->future_dist[p->time_step]) {
    remove_from_victim(p->v, b);
    b->weight = p->future_dist[p->time_step];
    add_to_victim(p->v, b);
  }
  p->time_step ++;
}

Block *pop_OPT(Policy *p) {
  for (uint64_t i = p->future_depth; i != -1; --i) {
    Block *vic = pop_last(p->v->weight_dll[i]);
    if (vic != NULL) 
      return vic;
  }
  return NULL;
}

void display_future(Policy *p) {
  for (uint64_t i = 0; i < p->future_depth; ++ i)
    printf("%" PRIu64 " ", p->future_refs[i]);
  printf("\n");
  for (uint64_t i = 0; i < p->future_depth; ++ i)
    printf("%" PRIu64 " ", p->future_dist[i]);
  printf("\n");
}