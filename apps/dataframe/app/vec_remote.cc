#include <iostream>
#include "common.h"
#include "greeting.h"
#include "remoteKVS.hpp"
#include <assert.h>
#include <string>

using namespace std;

static uint64_t max_size = (480 << 20);
static uint32_t cache_line_size = 8192;

int main(int argc, char * argv[]) 
{
	init(TRANS_TYPE_RC_SERVER, argv[1]);
  cout << "Start processing requests" << endl;
  KVS *kvs = new KVS(sbuf, max_size, cache_line_size);
  const unsigned int max_recvs = 64;
  const unsigned int inflights = max_recvs / 2;
  struct ibv_wc wc[max_recvs];
  unsigned int post_recvs = 0, poll_recvs = 0;
  // vanilla req-response 
  size_t req_size = sizeof(struct req) + cache_line_size;
  for (int i = 0; i < inflights; i++)
    recv_async((char *)rbuf + i*req_size, req_size);
  post_recvs += inflights;
  while (1)
  {
    int n = ibv_poll_cq(cq, inflights, wc);
    for (int i = 0; i < n; ++i)
    {
      if (wc[i].status == 0 && wc[i].wr_id != 0)
      {
        int idx = (poll_recvs ++) % max_recvs;
        struct req *r = (struct req *) ((char *)rbuf + idx*req_size);
        // cout << "Req: " << r->addr << " " << r->size << " " << r->type << endl;

        // if fetch type
        if (r->type) 
          kvs->handle_req_fetch(r);
        else
        {
          // cout << "Req: " << r->addr << " " << r->size << " " << r->type << endl;
          // recv playload
          // recv(r+1, sizeof(char) * cache_line_size);
          kvs->handle_req_update(r);
        }
      }
    }
    while (post_recvs < poll_recvs + inflights)
    {
      int idx = (post_recvs ++) % max_recvs;
      recv_async((char*) rbuf + idx*req_size, req_size);
    }
  }
}