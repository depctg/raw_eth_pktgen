#ifndef __REMOTE_POOL_H__
#define __REMOTE_POOL_H__

#include <stdlib.h>
#include <stdint.h>
#include "app.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* sbuf starting from base_sbuf is occupied by remote pools */
void manager_init();
void add_pool(int pid, unsigned linesize);

void process_cache_req(RPC_rrf_t *req);
void process_call_req(RPC_rrf_t *req);

void * deref_disagg_vaddr(uint64_t dvaddr);

void * remote_disagg_malloc(unsigned cache_id, size_t size);
void * remote_disagg_free(intptr_t addr);

#ifdef __cplusplus
}
#endif
#endif
