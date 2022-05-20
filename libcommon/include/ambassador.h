#ifndef __AMBASSADOR__
#define __AMBASSADOR__
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "uthash.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Entry point of communication with remote memory server */
typedef struct Ambassador
{
	char *reqs;
	char *line_pool;
	uint64_t cache_line_size;
	size_t req_size; /* sizeof(char) * line_size + sizeof(struct req) */

	uint32_t *snid;
	uint32_t ring_size;
	uint32_t front, end, frees;
} Ambassador;
Ambassador *newAmbassador(uint32_t ring_size, uint64_t cls, void *req_buffer, void *recv_buffer);

/* request for send buf */
uint32_t get_sid(Ambassador *a);
/* return send buf */
void ret_sid(Ambassador *a, uint32_t sid);

void fetch_sync(uint64_t addr, uint64_t rbuf_offset, Ambassador *a);
void update_sync(void *dat_buf, uint64_t addr, uint64_t size, Ambassador *a);

/* return wr_id of async wr */
uint64_t fetch_async(uint64_t addr, uint64_t rbuf_offset, Ambassador *a, uint32_t *sid);

#ifdef __cplusplus
}
#endif
#endif