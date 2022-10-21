#ifndef __REMOTE_POOL_H__
#define __REMOTE_POOL_H__

#include <stdlib.h>
#include <stdint.h>
#include "cache.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct cache_req_full {
  struct cache_req r;
  char* data[CACHE_LINE_LIMIT];
} R_REQ_TYPE;

/* sbuf starting from base_sbuf is occupied by remote pools */
void manager_init(void *base_sbuf);
void add_pool(int pid, uint64_t linesize);
void process_req(struct cache_req_full *req);

#ifdef __cplusplus
}
#endif
#endif
