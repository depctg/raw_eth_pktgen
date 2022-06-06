#include <iostream>
#include "common.h"
#include "greeting.h"
#include "remoteKVS.hpp"
#include <assert.h>
#include <string>

using namespace std;

constexpr static uint64_t max_size = 256 << 20;
constexpr static uint64_t cache_line_size = 1 << 10;

int main(int argc, char * argv[]) 
{
	init(TRANS_TYPE_RC_SERVER, argv[1]);
  cout << "Start processing requests" << endl;

  KVS *kvs = new KVS(sbuf, rbuf, max_size, cache_line_size);

  // cout << "In pool" << endl;
  // for (size_t i = 0; i < ary_size; ++i)
  // {
  //   cout << *((uint64_t*) kvs->cache_line_pool + i) << endl;
  // }

  kvs->serve();
}
