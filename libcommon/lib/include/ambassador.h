#ifndef __AMBASSADOR__
#define __AMBASSADOR__
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "uthash.h"
#include "mem_block.h"
#include "replacement.h"

#ifdef __cplusplus
extern "C"
{
#endif

enum CQ_OPT {SEND, RECV};

/* Hashtable of sending bufs
will return sid if consumed cq is a send_req */
typedef struct HashSid {
	int wr_id;
	uint32_t sid;
	UT_hash_handle hh;
} HashSid;

/* Hashtable of recv reqs
update block states when cq is polled */

typedef struct HashRid {
	int wr_id;
	Block *bptr;
	UT_hash_handle hh;
} HashRid;

/* Entry point of communication with remote memory server */
typedef struct Ambassador
{
	char *reqs;
	char *line_pool;
	uint64_t cache_line_size;
	size_t req_size; /* sizeof(char) * line_size + sizeof(struct req) */
	HashSid *wrid_sid;
	HashRid *wrid_bptr;

	uint32_t *snid;
	uint32_t ring_size;
	uint32_t front, end, frees;
} Ambassador;

Ambassador *newAmbassador(uint32_t ring_size, uint64_t cls, void *req_buffer, void *recv_buffer);

/* request for send buf */
uint32_t get_sid(Ambassador *a);
/* return send buf */
void ret_sid(Ambassador *a, uint32_t sid);

void fetch_sync(Block *b, Ambassador *a, Policy *p, uint8_t tag_shifts);
void update_sync(void *dat_buf, uint64_t addr, uint64_t size, Ambassador *a, Policy *p);

/* return wr_id of async wr */
uint64_t fetch_async(Block *b, Ambassador *a, Policy *p, uint8_t tag_shifts);

void cq_consumer(uint64_t wr_id, enum CQ_OPT opt, Ambassador *a, Policy *p);

void check_wq(Ambassador *a, Policy *p);

#ifdef __cplusplus
}
#endif
#endif