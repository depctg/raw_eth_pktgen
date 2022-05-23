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

void fetch_sync(Block *b, Ambassador *a, Policy *p, uint8_t tag_shifts)
{
	check_wq(a, p);
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
	cq_consumer(rk, RECV, a, p);
}

void update_sync(void *dat_buf, uint64_t addr, uint64_t size, Ambassador *a, Policy *p)
{
	check_wq(a, p);
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
	cq_consumer(sk, SEND, a, p);
}

uint64_t fetch_async(Block *b, Ambassador *a, Policy *p, uint8_t tag_shifts)
{
	check_wq(a, p);
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = b->tag << tag_shifts;
	r->size = a->cache_line_size;
	r->type = 1;
	// send_async(r, sizeof(struct req));
	uint64_t sk = send_async(r, a->req_size);
	uint64_t rk = recv_async(a->line_pool + b->rbuf_offset, a->cache_line_size);
	// printf("Fetch async tag %" PRIu64 ",wrids %" PRIu64 ", %" PRIu64 "\n", b->tag, sk, rk);
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
void cq_consumer(uint64_t wr_id, enum CQ_OPT opt, Ambassador *a, Policy *p){
	// printf("Polling wr_id %" PRIu64 ", op type %d\n", wr_id, opt);
	if (poll_id >= post_id)
		return;

	struct ibv_wc wcs[MAX_POLL];
	while (poll_id < post_id) {
		if (wr_id != 0 && opt == SEND && wr_id <= send_poll_id)
			break;
		if (wr_id != 0 && opt == RECV && wr_id <= recv_poll_id)
			break;
		int n = poll_cq(cq, MAX_POLL, wcs);
		for (int i = 0; i < n; ++ i) {
			if (wcs[i].status == IBV_WC_SUCCESS) {
				uint64_t polled_wr_id = wcs[i].wr_id;
				// printf("Polled %" PRIu64 ", opt: %" PRIu64 "\n", polled_wr_id, wcs[i].opcode);
				poll_id ++;
				if (wcs[i].opcode == IBV_WC_SEND) {
					// send complete
					HashSid *tgt;
					HASH_FIND_INT(a->wrid_sid, &polled_wr_id, tgt);
					if (tgt == NULL) {
						printf("Finished send is not recorded, cant return sid\n");
						exit(1);
					}
					ret_sid(a, tgt->sid);
					HASH_DEL(a->wrid_sid, tgt);
					free(tgt);
					if (polled_wr_id > send_poll_id) {
						send_poll_id = polled_wr_id;
					}
				} else {
					// recv complete
					HashRid *tgt;
					HASH_FIND_INT(a->wrid_bptr, &polled_wr_id, tgt);
					if (tgt == NULL) {
						printf("Finished recv is not recorded, cant update block state\n");
						exit(1);
					}
					Block *b = tgt->bptr;
					b->status = present;
					b->dirty = 0;
					b->weight = 0;
					// add_to_head(dll, b);
					p->fresh_add(p, b);
					HASH_DEL(a->wrid_bptr, tgt);
					free(tgt);
					if (polled_wr_id > recv_poll_id) {
						recv_poll_id = polled_wr_id;
					}
				}
			}
		}
	}
}

void check_wq(Ambassador *a, Policy *p) {
	if (send_post_id >= send_poll_id + CQ_NUM_DESC / 2 || recv_post_id >= recv_post_id + CQ_NUM_DESC / 2)
		cq_consumer(0, RECV, a, p); // consume all wr
}
