#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common.h"
#include "remote_pool.h"
#include "cache.h"
#include "cache_internal.h"
#include "helper.h"

#define POOL_MAX_BLOCK 2048
const int block_size_bits = 21;
const uint64_t BLOCK_SIZE = (1U << block_size_bits);

struct remote_pool {
  uint64_t linesize;
  // bitmap of blocks in this pool
  // indicate maximum mem size of BLOCK_SIZE * POOL_MAX_BLOCK for each pool
  char *block_base[POOL_MAX_BLOCK]; 
};

static const int initial_blocks = 64;
static uint64_t block_size_mask = BLOCK_SIZE - 1;
static struct remote_pool pools[OPT_NUM_CACHE + CACHE_ID_OFFSET];
static char *pool_base;
static uint64_t free_start = 0;

/* Remote pool layout 
pool is a series of discontinuous blocks of the same size
each pool has a mapping from pool-based virtual address to the actual block
*/

void manager_init(void *base_sbuf) {
  assert(is_pow2(BLOCK_SIZE) && BLOCK_SIZE > 0);
  pool_base = base_sbuf;
}

#define get_pool_meta(id, field) \
  ((pools[id]).field)

void add_pool(int pid, uint64_t linesize) {
  assert(pid < OPT_NUM_CACHE + CACHE_ID_OFFSET);
  assert(linesize <= PAYLOAD_LIMIT);
  assert(is_pow2(linesize) && linesize > 0);

  pools[pid].linesize = linesize;

  for (int i = 0; i < POOL_MAX_BLOCK; ++i) {
    pools[pid].block_base[i] = NULL;
    if (i < initial_blocks) {
      pools[pid].block_base[i] = pool_base + free_start;
      free_start += BLOCK_SIZE;
    }
  }
}

/* map tag to the corresponding start address of this line */
static inline void *tag_mapping(uint64_t tag, uint8_t cache_id) {
  uint64_t block_id = ( tag >> block_size_bits );
  if (block_id >= POOL_MAX_BLOCK) {
    fprintf(stderr, "Request tag %lx out of bound\nCurrent remote memory limit: %lu bytes", tag, BLOCK_SIZE * POOL_MAX_BLOCK);
    exit(1);
  }
  char *base = get_pool_meta(cache_id, block_base)[block_id];
  if (likely(base != NULL)) {
    dprintf("Access block %lu", block_id);
    // if pre-assigned block for this tag
    return base + (tag & block_size_mask);
  } else {
    dprintf("Allocate new block %lu", block_id);
    // need to allocate another block in this pool
    get_pool_meta(cache_id, block_base)[block_id] = pool_base + free_start;
    free_start += BLOCK_SIZE;
    return get_pool_meta(cache_id, block_base)[block_id] + (tag & block_size_mask);
  }
}

// inline?
void process_cache_req(RPC_rrf_t *req_full) {
  cache_req_t req = req_full->rr.cache_r_header;
  uint8_t type = req_full->rr.op_code;
  uint8_t cache_id = req.cache_id;
  uint64_t cache_line_size = get_pool_meta(cache_id, linesize);

  // fdprintf("cid %d, tag %lu, tag2 %lu, op %d", req->cache_id, req->tag, req->tag2, req->type);

  if (type == CACHE_REQ_READ) {
    uint64_t read_offset = req.tag;
    dprintf("READ: pool %d, tag %lu", cache_id, read_offset);
    // printf("READ: pool %d, tag %lu\n", cache_id, read_offset);
    send_async(tag_mapping(read_offset, cache_id), cache_line_size);
  }
  if (type == CACHE_REQ_WRITE) {
    uint64_t write_offset = req.tag;
    dprintf("WRITE: pool %d, tag %lu", cache_id, write_offset);
    // printf("WRITE: pool %d, tag %lu\n", cache_id, write_offset);
    // read_as_exp(c, tag_mapping(write_offset, cache_id), cache_line_size);
    // TODO: zero-copy
    memcpy(tag_mapping(write_offset, cache_id), req_full->data_seg_base, cache_line_size);
  }
  if (type == CACHE_REQ_EVICT) {
    uint64_t read_offset = req.tag;
    uint64_t write_offset = req.tag2;
    dprintf("EVICT: pool %d, write %lu, read %lu", cache_id, write_offset, read_offset);
    // printf("EVICT: pool %d, write %lu, read %lu\n", cache_id, write_offset, read_offset);
    send_async(tag_mapping(read_offset, cache_id), cache_line_size);
    // TODO: zero-copying
    memcpy(tag_mapping(write_offset, cache_id), req_full->data_seg_base, cache_line_size);
  }
}

void process_call_req(RPC_rrf_t *req) {
  UNUSED(req);
}
