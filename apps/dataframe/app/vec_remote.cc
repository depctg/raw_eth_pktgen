#include <iostream>
#include "common.h"
#include "greeting.h"
#include "remoteKVS.hpp"
#include <assert.h>
#include <string>
#include <getopt.h>

using namespace std;

static struct option long_options[] = {
    {"addr", required_argument, 0, 0},
    {"cache_size", required_argument, 0, 0},
    {"cache_line_size", required_argument, 0, 0},
    {0, 0, 0, 0}
};

int main(int argc, char * argv[]) 
{
    char * addr = 0;
    uint64_t prefetch_size = 0;
    uint64_t cache_line_size = 8192;
    uint64_t cache_size = (480 << 20);
    uint64_t index_size = 100;

    int opt= 0, long_index =0;
    while ((opt = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1) {
        switch (long_index) {
            case 0:
                addr = optarg;
                break;
            case 1:
                cache_size = atoll(optarg);
                break;
            case 2:
                cache_line_size = atoll(optarg);
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                return -1;
        }
    }
    if (!addr) return -1;

  init(TRANS_TYPE_RC_SERVER, addr);
  KVS *kvs = new KVS(sbuf, rbuf, cache_size, cache_line_size);

  size_t req_size = sizeof(struct req) + cache_line_size;

  while (1)
  {
    recv(rbuf, req_size);
    struct req *r = (struct req *) rbuf;
    // const char *type = r->type == 1 ? "Fetch" : "Update";
    // cout << "Req " << r->addr << ", size " << r->size << ", type " << type << endl;

    // if fetch type
    if (r->type) 
      kvs->handle_req_fetch(r);
    else
    {
      kvs->handle_req_update(r);
    }
  }

  // cout << "Start processing requests" << endl;
  // const unsigned int max_recvs = 64;
  // const unsigned int inflights = max_recvs / 2;
  // struct ibv_wc wc[max_recvs];
  // unsigned int post_recvs = 0, poll_recvs = 0;
  // // vanilla req-response 
  // size_t req_size = sizeof(struct req) + cache_line_size;
  // for (int i = 0; i < inflights; i++)
  //   recv_async((char *)rbuf + i*req_size, req_size);
  // post_recvs += inflights;
  // while (1)
  // {
  //   int n = ibv_poll_cq(cq, inflights, wc);
  //   for (int i = 0; i < n; ++i)
  //   {
  //     if (wc[i].status == 0 && wc[i].wr_id != 0)
  //     {
  //       int idx = (poll_recvs ++) % max_recvs;
  //       struct req *r = (struct req *) ((char *)rbuf + idx*req_size);
  //       // cout << "Req: " << r->addr << " " << r->size << " " << r->type << endl;

  //       // if fetch type
  //       if (r->type) 
  //         kvs->handle_req_fetch(r);
  //       else
  //       {
  //         // cout << "Req: " << r->addr << " " << r->size << " " << r->type << endl;
  //         // recv playload
  //         // recv(r+1, sizeof(char) * cache_line_size);
  //         kvs->handle_req_update(r);
  //       }
  //     }
  //   }
  //   while (post_recvs < poll_recvs + inflights)
  //   {
  //     int idx = (post_recvs ++) % max_recvs;
  //     recv_async((char*) rbuf + idx*req_size, req_size);
  //   }
  // }
}
