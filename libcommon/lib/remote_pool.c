#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "remote_pool.h"
#include "cache.h"

static struct remote_pool pools[OPT_NUM_CACHE];
static char *pool_base;
static uint64_t free_start = 0;

void manager_init(void *base_sbuf) {
  pool_base = base_sbuf;
}

#define get_pool_meta(id, field) \
  ((pools[id]).field)

void add_pool(int pid, uint64_t linesize) {
  assert(pid < OPT_NUM_CACHE);
  assert(linesize <= CACHE_LINE_LIMIT);
  assert(is_pow2(linesize));

  for (int i = 0; i < 1024; ++i)
    pools[pid].block_base[i] = NULL;
  pools[pid].linesize = linesize;
  pools[pid].block_base[0] = pool_base + free_start;
  free_start += BLOCK_SIZE;
}

/* map tag to the corresponding start address of this line */
void *tag_mapping(uint64_t tag, uint8_t cache_id) {
  uint64_t block_id = tag / BLOCK_SIZE;
  char *base = get_pool_meta(cache_id, block_base)[block_id];
  if (base != NULL) {
    dprintf("Access block %lu", block_id);
    // if pre-assigned block for this tag
    return base + (tag % BLOCK_SIZE);
  } else {
    dprintf("Allocate new block %lu", block_id);
    // need to allocate another block in this pool
    get_pool_meta(cache_id, block_base)[block_id] = pool_base + free_start;
    free_start += BLOCK_SIZE;
    return get_pool_meta(cache_id, block_base)[block_id] + (tag % BLOCK_SIZE);
  }
}

void process_req(struct cache_req_full *req) {
  uint8_t cache_id = req->r.cache_id;
  uint64_t cache_line_size = get_pool_meta(cache_id, linesize);

  if (req->r.type == CACHE_REQ_READ) {
    uint64_t read_offset = req->r.tag & REQ_META_MASK;
    dprintf("READ: pool %d, offset %lu, line %lu", cache_id, read_offset, cache_line_size);
    send_async(tag_mapping(read_offset, cache_id), cache_line_size);
  }
  if (req->r.type == CACHE_REQ_WRITE) {
    uint64_t write_offset = req->r.tag & REQ_META_MASK;
    dprintf("WRITE: pool %d, offset %lu, line %lu", cache_id, write_offset, cache_line_size);
    memcpy(tag_mapping(write_offset, cache_id), req->data, cache_line_size);
  } 
  if (req->r.type == CACHE_REQ_EVICT) {
    uint64_t read_offset = req->r.tag & REQ_META_MASK;
    uint64_t write_offset = req->r.newtag & REQ_META_MASK;
    dprintf("EVICT: pool %d, write %lu, read %lu, line %lu", cache_id, write_offset, read_offset, cache_line_size);
    send_async(tag_mapping(read_offset, cache_id), cache_line_size);
    memcpy(tag_mapping(write_offset, cache_id), req->data, cache_line_size);
  }
}

