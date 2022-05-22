#include <iostream>
#include "common.h"
#include "greeting.h"
#include "remoteKVS.hpp"
#include <assert.h>
#include <string>

using namespace std;

constexpr static uint64_t max_size = 256 << 20;
constexpr static uint64_t array_size = 1 << 10;
constexpr static uint64_t cache_line_size = 1 << 8;

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
    uint64_t offset = kvs->sbuf_manager->claim();
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

  KVS *kvs = new KVS(sbuf, rbuf, max_size, cache_line_size);
  app_init(kvs);

  // cout << "In pool" << endl;
  // for (size_t i = 0; i < ary_size; ++i)
  // {
  //   cout << *((uint64_t*) kvs->cache_line_pool + i) << endl;
  // }

  kvs->serve();
}
