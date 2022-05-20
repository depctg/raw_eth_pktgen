#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#include "ambassador.h"
#include "common.h"
#include "greeting.h"

Ambassador *newAmbassador(uint32_t ring_size, uint64_t cls, void *req_buffer, void *recv_buffer)
{
	Ambassador *a = (Ambassador *) malloc(sizeof(Ambassador));
	a->ring_size = ring_size;
	a->cache_line_size = cls;
	a->req_size = sizeof(struct req) + sizeof(char) * cls;
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

void fetch_sync(uint64_t addr, uint64_t rbuf_offset, Ambassador *a)
{
	// printf("Fetch addr: %" PRIu64 "\n", addr);
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = addr;
	r->size = a->cache_line_size;
	r->type = 1;
	send(r, a->req_size);
	recv(a->line_pool + rbuf_offset, a->cache_line_size);
	ret_sid(a, send_buf_nid);
}

void update_sync(void *dat_buf, uint64_t addr, uint64_t size, Ambassador *a)
{
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = addr;
	r->size = size;
	r->type = 0;
	memcpy(r+1, dat_buf, size);
	// send(r, sizeof(struct req));
	// send(r+1, a->cache_line_size);
	send(r, a->req_size);
	ret_sid(a, send_buf_nid);
}

uint64_t fetch_async(uint64_t addr, uint64_t rbuf_offset, Ambassador *a, uint32_t *sid)
{
	uint32_t send_buf_nid = get_sid(a);
	struct req *r = (struct req *) (a->reqs + send_buf_nid * a->req_size);
	r->addr = addr;
	r->size = a->cache_line_size;
	r->type = 1;
	send_async(r, sizeof(struct req));
	return recv_async(a->line_pool + rbuf_offset, a->cache_line_size);
}