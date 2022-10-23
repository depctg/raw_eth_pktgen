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
void manager_init(void *base_sbuf);
void add_pool(int pid, uint64_t linesize);

void process_cache_req(RPC_rrf_t *req);
void process_call_req(RPC_rrf_t *req);

#ifdef __cplusplus
}
#endif
#endif
