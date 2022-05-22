#ifndef __MEM_SLICER__
#define __MEM_SLICER__

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#ifdef __cplusplus
extern "C"
{
#endif

// A queue that represents free slots in rbuf
typedef struct FreeQueue
{
	uint64_t capacity;
	uint32_t front, end, frees;
	uint64_t* slots;
} FreeQueue;

FreeQueue *initQueue(uint64_t max_size, uint64_t cache_line_size);

int isFull(FreeQueue *fq);
int isEmpty(FreeQueue *fq);

// claim a room for cache line, return 0 if no free
int claim(FreeQueue *fq, uint64_t *offset);

// return a cache line room for future use
void reclaim(FreeQueue *fq, uint64_t *offset);

#ifdef __cplusplus
}
#endif
#endif