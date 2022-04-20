#include <iostream>
#include "../common.h"
#include "../greeting.h"
#include "../remoteKVS.hpp"
#include <assert.h>
#include <string>

using namespace std;

static uint64_t max_size = (480 << 20);
static uint32_t cache_line_size = 16384 * 2;

int main(int argc, char * argv[]) 
{
	init(TRANS_TYPE_RC_SERVER, argv[1]);
  cout << "Start processing requests" << endl;
  KVS *kvs = new KVS(sbuf, max_size, cache_line_size);

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
      // cout << "Req: " << r->addr << " " << r->size << " " << r->type << endl;
      // recv playload
      recv(r+1, sizeof(char) * cache_line_size);
      kvs->handle_req_update(r);
    }
  }
}
