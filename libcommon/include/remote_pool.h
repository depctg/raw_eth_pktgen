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
extern uint64_t local_remote_delimiter;
void manager_init();
void add_pool(int pid, unsigned linesize);

void process_cache_req(RPC_rrf_t *req);
void process_channel_req(RPC_rrf_t *req_full);
void process_call_req(RPC_rrf_t *req);

void * deref_disagg_vaddr(uint64_t dvaddr);

#ifdef __cplusplus
}
#endif
#endif
