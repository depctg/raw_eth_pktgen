#include <iostream>
#include "../common.h"
#include "../greeting.h"
#include "../remoteKVS.hpp"
#include <assert.h>
#include <string>

using namespace std;

static uint64_t max_size = 256 << 20;
static uint32_t cache_line_size = 1024;
static uint32_t ary_size = 4 << 20;

// data resides in sbuf for non-copy
void app_init(KVS *kvs, uint32_t ary_size)
{
  // manutal insertion
  uint64_t *dummy = new (kvs->cache_line_pool) uint64_t[ary_size];
  const uint32_t item_perline = cache_line_size * sizeof(char) / sizeof(uint64_t);
  const size_t num_lines = ary_size / item_perline;
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
  app_init(kvs, ary_size);
  // cout << "In pool" << endl;
  // for (size_t i = 0; i < ary_size; ++i)
  // {
  //   cout << *((uint64_t*) kvs->cache_line_pool + i) << endl;
  // }
  // notify start
  send((char *)sbuf + max_size + sizeof(uint64_t), 1);
  // vanilla req-response 
  while (1)
  {
    recv(rbuf, sizeof(struct req));
    struct req *r = (struct req *) rbuf;
    // cout << "Req: " << r->addr << " " << r->size << " " << r->type << endl;

    // if fetch type
    if (r->type) 
    {
      kvs->handle_req_fetch(r);
    }
    else
    {
      // recv playload
      recv((char*)rbuf + sizeof(struct req), cache_line_size);
      kvs->handle_req_update(r);
    }
  }
}
