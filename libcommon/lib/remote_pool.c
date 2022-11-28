#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common.h"
#include "remote_pool.h"
#include "cache.h"
#include "cache_internal.h"
#include "helper.h"
#include "channel_internal.h"

typedef void (*rpc_service_t)(void *arg, void *ret);

// populate by offload obj
extern rpc_service_t *services;
extern void init_rpc_services();

static unsigned linesize_cfgs[OPT_NUM_CACHE + CACHE_ID_OFFSET];
static char *rpc_ret_base;

static char *baseva_shared_addrspace;
uint64_t local_remote_delimiter = 2ULL<<48;

/* Remote pool layout 
pool is a series of discontinuous blocks of the same size
each pool has a mapping from pool-based virtual address to the actual block
*/

void manager_init() {

  rpc_ret_base = sbuf;
  baseva_shared_addrspace = (char *) sbuf + RPC_RET_LIMIT;

  // set cache configurations
  char *cfg_path = (getenv("CACHE_CFG"));
  if (!cfg_path) {
    printf("Set CACHE_CFG env to path to the configuration file\n");
    exit(1);
  }
  FILE *cache_cfg = fopen(cfg_path, "r");
  if (!cache_cfg) {
    printf("Cannot open configuration file %s\n", cfg_path);
    exit(1);
  }

  while (1) {
    int cid; 
    uint64_t cache_size;
    uint64_t remote_usage_limit;
    unsigned line_size;

    int h = fgetc(cache_cfg);
    if (h == '#') {
      fscanf(cache_cfg, "%*[^\n]\n");
      continue;
    } else if (h == EOF)
      break;

    if (fscanf(cache_cfg, " %d %lu %lu %u\n", &cid, &cache_size, &remote_usage_limit, &line_size) == EOF) break;
    line_size = align_with_pow2(line_size);
    add_pool(cid, line_size);
    printf("Regist cache %d, line size %u bytes\n", cid, line_size);
  }
  fclose(cache_cfg);

  // register rpc services
  init_rpc_services();
  *(uint64_t *)sbuf = (intptr_t) baseva_shared_addrspace;
  send(sbuf, sizeof(uint64_t));
  printf("Remote server: cache base addr = %p\n", baseva_shared_addrspace);
}

void add_pool(int pid, unsigned line_size) {
  assert(pid < OPT_NUM_CACHE + CACHE_ID_OFFSET);
  assert(line_size <= PAYLOAD_LIMIT);
  assert(is_pow2(line_size) && line_size > 0);

  linesize_cfgs[pid] = line_size;
}

// TODO: ASSERT limit
// map tag to the corresponding start address of this line
static inline void *addr_mapping(uint64_t addr) {
  if (addr >= local_remote_delimiter)
    return (void *)(addr - local_remote_delimiter);
  else
    return baseva_shared_addrspace + addr;
}

void * deref_disagg_vaddr(uint64_t cid_dvaddr) {
  virt_addr_t ser = {.ser = cid_dvaddr};
  uint64_t dvaddr = ser.addr;
  cache_t cache = ser.cache;
  dprintf("remotely access %d addr: %lu", cache, dvaddr);
  UNUSED(cache);
  return addr_mapping(dvaddr);
}

// inline?
void process_cache_req(RPC_rrf_t *req_full) {
  cache_req_t req = req_full->rr.cache_r_header;
  uint8_t type = req_full->rr.op_code;
  uint8_t cache_id = req.cache_id;
  uint64_t cache_line_size = linesize_cfgs[cache_id];

  // fdprintf("cid %d, tag %lu, tag2 %lu, op %d", req->cache_id, req->tag, req->tag2, req->type);

  if (type == CACHE_REQ_READ) {
    uint64_t read_offset = req.tag;
    dprintf("READ: pool %d, tag %lu", cache_id, read_offset);
    // printf("READ: pool %d, tag %lu\n", cache_id, read_offset);
    send_async(addr_mapping(read_offset), cache_line_size);
  }
  else if (type == CACHE_REQ_WRITE || type == CACHE_REQ_FLUSH) {
    uint64_t write_offset = req.tag;
    dprintf("WRITE: pool %d, tag %lu", cache_id, write_offset);
    // printf("WRITE: pool %d, tag %lu\n", cache_id, write_offset);
    // read_as_exp(c, tag_mapping(write_offset, cache_id), cache_line_size);
    // TODO: zero-copy
    memcpy(addr_mapping(write_offset), req_full->data_seg_base, cache_line_size);
  }
  else if (type == CACHE_REQ_EVICT) {
    uint64_t read_offset = req.tag;
    uint64_t write_offset = req.tag2;
    dprintf("EVICT: pool %d, write %lu, read %lu", cache_id, write_offset, read_offset);
    // printf("EVICT: pool %d, write %lu, read %lu\n", cache_id, write_offset, read_offset);
    send_async(addr_mapping(read_offset), cache_line_size);
    // TODO: zero-copying
    memcpy(addr_mapping(write_offset), req_full->data_seg_base, cache_line_size);
  }
  else {
    fprintf(stderr, "get unknown req type: %d\n", type);
  }
}

void process_channel_req(RPC_rrf_t *req_full) {
  side_channel_t req = req_full->rr.side_r_header;
  uint8_t type = req_full->rr.op_code;
  if (type == SIDE_READ) {
    dprintf("Channel READ addr %lu size %d", req.raddr, req.rsize);
    // printf("Channel READ addr %lu size %d\n", req.raddr, req.rsize);
    send_async(addr_mapping(req.raddr), req.rsize);
  }
  else if (type == SIDE_WRITE) {
    dprintf("Channel WRITE addr %lu size %d", req.waddr, req.wsize);
    memcpy(addr_mapping(req.waddr), req_full->data_seg_base, req.wsize);
  }
  else if (type == SIDE_EVICT) {
    dprintf("Channel EVICT %lu %d -> %lu %d ", req.waddr, req.wsize, req.raddr, req.rsize);
    send_async(addr_mapping(req.raddr), req.rsize);
    memcpy(addr_mapping(req.waddr), req_full->data_seg_base, req.wsize);
  }
  else {
    fprintf(stderr, "get unknown req type: %d\n", type);
  }
}

void process_call_req(RPC_rrf_t *req) {
  int fid = req->rr.call_r_header.procedure_id;
  dprintf("Calling %d service", fid);

  void *arg = req->rr.call_r_header.arg_size ? req->data_seg_base : NULL;
  void *ret = req->rr.call_r_header.ret_size ? rpc_ret_base : NULL;
  services[fid](arg, ret);
  if (req->rr.call_r_header.ret_size)
    send_async(rpc_ret_base, req->rr.call_r_header.ret_size);
}
