#ifndef __REMOTE_POOL_H__
#define __REMOTE_POOL_H__

#include <stdlib.h>
#include <stdint.h>
#include "cache.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BLOCK_SIZE (4 << 20)
/* Remote pool layout 
pool is a series of discontinuous blocks of the same size
each pool has a mapping from pool-based virtual address to the 
actual block
*/
struct remote_pool {
  uint64_t linesize;
  char *block_base[1024]; // bitmap of blocks for this pool
};

struct cache_req_full {
  struct cache_req r;
  uint8_t data[CACHE_LINE_LIMIT];
};

/* sbuf starting from base_sbuf is occupied by remote pools */
void manager_init(void *base_sbuf);
void add_pool(int pid, uint64_t linesize);
void process_req(struct cache_req_full *req);

#ifdef __cplusplus
}
#endif
#endif
