#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#include "ambassador.h"
#include "mem_block.h"
#include "common.h"
#include "greeting.h"

Ambassador *newAmbassador(uint32_t ring_size, uint64_t cls, void *req_buffer, void *recv_buffer)
{
	Ambassador *a = (Ambassador *) malloc(sizeof(Ambassador));
	a->ring_size = ring_size;
	a->cache_line_size = cls;
	a->req_size = sizeof(struct req) + sizeof(char) * cls;
	a->wrid_sid = NULL;
	a->wrid_bptr = NULL;

	a->snid = (uint32_t *) malloc(sizeof(uint32_t) * ring_size);
	for (uint32_t i = 0; i < ring_size; ++i)
	{
		a->snid[i] = i;
	}
	a->front = 0;
	a->frees = a->ring_size;
	a->end = ring_size - 1;
	a->reqs = (char *) req_buffer;
	a->line_pool = (char *) recv_buffer;
	return a;
}

uint32_t get_sid(Ambassador *a)
{
	if (!a->frees)
	{
		printf("No free send buffer slot\n");
		// TODO: return wait signal
		exit(1);
	}
	uint32_t sid = a->snid[a->front];
	a->front = (a->front + 1) % a->ring_size;
	a->frees -= 1;
	return sid;
}

void ret_sid(Ambassador *a, uint32_t sid)
{
	if (a->frees == a->ring_size) return;
	a->end = (a->end + 1) % a->ring_size;
	a->snid[a->end] = sid;
	a->frees += 1;
}

void fetchedPrint(char *line, uint32_t cache_line_size)
{
	uint64_t *vline = (uint64_t *) line;
	printf("line: \n");
	for (size_t i = 0; i < cache_line_size * sizeof(char) / sizeof(uint64_t); ++ i)
	{
		printf("%" PRIu64 " ", *(vline + i));
	}
	printf("end-----\n");
}

void fetch_sync(Block *b, Ambassador *a, BlockDLL *dll, uint8_t tag_shifts)
{
	uint64_t addr = b->tag << tag_shifts;
	// printf("Fetch addr: %" PRIu64 "\n", addr);
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = addr;
	r->size = a->cache_line_size;
	r->type = 1;
	uint64_t sk = send_async(r, a->req_size);
	uint64_t rk = recv_async(a->line_pool + b->rbuf_offset, a->cache_line_size);
	// update hashtable
	HashSid *hs = (HashSid *) malloc(sizeof(HashSid));
	hs->wr_id = (int) sk;
	hs->sid = send_buf_nid;
	HASH_ADD_INT(a->wrid_sid, wr_id, hs); 

	HashRid *hr = (HashRid *) malloc(sizeof(HashRid));
	hr->wr_id = (int) rk;
	hr->bptr = b;
	HASH_ADD_INT(a->wrid_bptr, wr_id, hr);
	b->wr_id = rk;
	// fetchedPrint(a->line_pool + b->rbuf_offset, a->cache_line_size);
	cq_consumer(rk, a, dll);
}

void update_sync(void *dat_buf, uint64_t addr, uint64_t size, Ambassador *a, BlockDLL *dll)
{
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = addr;
	r->size = size;
	r->type = 0;
	memcpy(r+1, dat_buf, size);
	// send(r, sizeof(struct req));
	// send(r+1, a->cache_line_size);
	uint64_t sk = send_async(r, a->req_size);
	// update hashtable
	HashSid *hs = (HashSid *) malloc(sizeof(HashSid));
	hs->wr_id = (int) sk;
	hs->sid = send_buf_nid;
	HASH_ADD_INT(a->wrid_sid, wr_id, hs); 
	cq_consumer(sk, a, dll);
}

uint64_t fetch_async(Block *b, Ambassador *a, uint8_t tag_shifts)
{
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = b->tag << tag_shifts;
	r->size = a->cache_line_size;
	r->type = 1;
	// send_async(r, sizeof(struct req));
	uint64_t sk = send_async(r, a->req_size);
	uint64_t rk = recv_async(a->line_pool + b->rbuf_offset, a->cache_line_size);
	// update hashtable
	HashSid *hs = (HashSid *) malloc(sizeof(HashSid));
	hs->wr_id = (int) sk;
	hs->sid = send_buf_nid;
	HASH_ADD_INT(a->wrid_sid, wr_id, hs); 

	HashRid *hr = (HashRid *) malloc(sizeof(HashRid));
	hr->wr_id = (int) rk;
	hr->bptr = b;
	HASH_ADD_INT(a->wrid_bptr, wr_id, hr);
	b->wr_id = rk;
	return rk;
}

/* if wr_id is 0, consume the entire cq */
void cq_consumer(uint64_t wr_id, Ambassador *a, BlockDLL *dll) {
	if (wr_id != 0 && poll_id >= wr_id)
		return;

	struct ibv_wc wcs[MAX_POLL];
	while ((wr_id == 0 || poll_id < wr_id) && poll_id < post_id) {
		int n = ibv_poll_cq(cq, MAX_POLL, wcs);
		for (int i = 0; i < n; ++ i) {
			if (wcs[i].status == 0 && wcs[i].wr_id > poll_id) {
				uint64_t wr_id = wcs[i].wr_id;
				poll_id = wr_id;
				if (wcs[i].opcode == IBV_WC_SEND) {
					// send complete
					HashSid *tgt;
					HASH_FIND_INT(a->wrid_sid, &wr_id, tgt);
					if (tgt == NULL) {
						printf("Finished send is not recorded, cant return sid\n");
						exit(1);
					}
					ret_sid(a, tgt->sid);
					HASH_DEL(a->wrid_sid, tgt);
					free(tgt);
				} else {
					// recv complete
					HashRid *tgt;
					HASH_FIND_INT(a->wrid_bptr, &wr_id, tgt);
					if (tgt == NULL) {
						printf("Finished recv is not recorded, cant update block state\n");
						exit(1);
					}
					Block *b = tgt->bptr;
					b->status = present;
					b->dirty = 0;
					b->weight = 0;
					add_to_head(dll, b);
					HASH_DEL(a->wrid_bptr, tgt);
					free(tgt);
				}
			}
		}
	}
}