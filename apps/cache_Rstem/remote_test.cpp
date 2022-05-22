#include <iostream>
#include "common.h"
#include "greeting.h"
#include "remoteKVS.hpp"
#include <assert.h>
#include <string>

using namespace std;

constexpr static uint64_t max_size = 256 << 20;
constexpr static uint64_t array_size = 1 << 10;
constexpr static uint32_t cache_line_size = 1 << 10;

// data resides in sbuf for non-copy
void app_init(KVS *kvs)
{
  // manutal insertion
  uint64_t *dummy = new (kvs->cache_line_pool) uint64_t[array_size];
  const uint32_t item_perline = cache_line_size * sizeof(char) / sizeof(uint64_t);
  const size_t num_lines = array_size / item_perline;
  for (size_t i = 0; i < num_lines; i++)
  {
    for (uint32_t j = 0; j < item_perline; j++)
    {
      dummy[i*item_perline + j] = i*item_perline + j;
    }
    // offset on sbuf (bytes)
    uint64_t offset = kvs->slotManager->claim();
    // cout << offset << endl;
    assert(offset == i*cache_line_size);
    uint64_t tag = ((offset * sizeof(char)) & kvs->addr_mask) >> kvs->tag_shifts;
    kvs->map[tag] = offset;
  }
  cout << "app init success" << endl;
}

int main(int argc, char * argv[]) 
{
	init(TRANS_TYPE_RC_SERVER, argv[1]);
  cout << "Start processing requests" << endl;
  KVS *kvs = new KVS(sbuf, max_size, cache_line_size);
  app_init(kvs);

  // cout << "In pool" << endl;
  // for (size_t i = 0; i < ary_size; ++i)
  // {
  //   cout << *((uint64_t*) kvs->cache_line_pool + i) << endl;
  // }

  const unsigned int max_recvs = 64;
  const unsigned int inflights = max_recvs / 2;
  struct ibv_wc wc[max_recvs];
  unsigned int post_recvs = 0, poll_recvs = 0;

  size_t req_size = sizeof(struct req) + cache_line_size;
  for (int i = 0; i < inflights; i++)
    recv_async((char *) rbuf + i * req_size, req_size);
  post_recvs += inflights;
  while (1)
  {
    int n = poll_cq(cq, inflights, wc);
    for (int i = 0; i < n; ++i)
    {
      if (wc[i].status == 0 && wc[i].opcode == IBV_WC_RECV)
      {
        int idx = (poll_recvs ++) % max_recvs;
        struct req *r = (struct req *) ((char *) rbuf + idx * req_size);
        // const char *type = r->type == 1 ? "Fetch" : "Update";
        // cout << "Req " << r->addr << ", tag " << (r->addr >> kvs->tag_shifts)  << ", size " << r->size << ", type " << type << endl;
        if (r->type) 
          kvs->handle_req_fetch(r);
        else
          kvs->handle_req_update(r);
      }
    }
    while (post_recvs < poll_recvs + inflights)
    {
      int idx = (post_recvs ++) % max_recvs;
      recv_async((char *) rbuf + idx * req_size, req_size);
    }
  }
}
