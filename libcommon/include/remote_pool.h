#ifndef __REMOTE_POOL_H__
#define __REMOTE_POOL_H__

#include <stdlib.h>
#include <stdint.h>
#include "cache.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define POOL_MAX_BLOCK 2048
const int block_size_bits = 21;
const uint64_t BLOCK_SIZE = (1U << block_size_bits);

/* Remote pool layout 
pool is a series of discontinuous blocks of the same size
each pool has a mapping from pool-based virtual address to the 
actual block
*/
struct remote_pool {
  uint64_t linesize;
  char *block_base[POOL_MAX_BLOCK]; // bitmap of blocks for this pool
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
