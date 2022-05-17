#include <iostream>
#include <memory>
#include <cstdlib>
#include "common.h"
#include "greeting.h"
#include "remoteKVS.hpp"

constexpr static uint64_t max_size = 2 << 30;
constexpr static uint64_t cache_line_size = 1000;
constexpr static uint64_t sbuf_ofst = 4 << 20;

using namespace std;
int main(int argc, char * argv[]) {
  if (argc != 2) {
      printf("Usage: %s <connection-key\n", argv[0]);
      exit(1);
  }
	init(TRANS_TYPE_RC_SERVER, argv[1]);

  // init remote KVS with sbuf, total size, cache line size
  // sbuf is used for write reqest from clients
  // so that send can be zero-copy
  KVS *kvs = new KVS((char *)sbuf + sbuf_ofst, max_size, cache_line_size);

  const unsigned int max_recvs = 64;
  const unsigned int inflights = max_recvs / 2;
  struct ibv_wc wc[max_recvs];
  unsigned int post_recvs = 0, poll_recvs = 0;
  size_t req_size = sizeof(struct req) + cache_line_size;
  for (int i = 0; i < inflights; i++)
    recv_async((char *)rbuf + i*req_size, req_size);
  post_recvs += inflights;
  while (1)
  {
    int n = poll_cq(cq, inflights, wc);
    for (int i = 0; i < n; ++i)
    {
      if (wc[i].status == 0 && wc[i].wr_id != 0)
      {
        int idx = (poll_recvs ++) % max_recvs;
        struct req *r = (struct req *) ((char *)rbuf + idx*req_size);
        const char *type = r->type == 1 ? "Fetch" : "Update";
        cout << "Req " << r->addr << ", size " << r->size << ", type " << type << endl;

        // if fetch type
        if (r->type) 
          kvs->handle_req_fetch(r);
        else
        {
          // print playload
          cout << "Payload: " << endl;
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
