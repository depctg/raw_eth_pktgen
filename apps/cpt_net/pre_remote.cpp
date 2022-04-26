#include <iostream>
#include "common.h"
#include "greeting.h"
#include <assert.h>
#include <string>
#include "sleepus.hpp"

using namespace std;

constexpr uint64_t sbuf_reserve = 480 << 20;
constexpr uint64_t batch_size = 4096;
constexpr uint64_t net_lat = 0; /*us*/

void sbuf_populate(uint64_t offset, size_t size, void *dat_buf)
{
  memcpy((char *) sbuf + offset, dat_buf, size);
}

void print_payload(void *dat_buf, size_t size)
{
  cout << "Payload: ";
  uint64_t *ps = (uint64_t *)dat_buf;
  for (int i = 0; i < size / sizeof(uint64_t); ++i)
  {
    cout << ps[i] << " ";
  }
  cout << endl;
}

int main(int argc, char **argv)
{
  init(TRANS_TYPE_RC_SERVER, argv[1]);
  cout << "Start processing requests" << endl;

  const unsigned int max_recvs = 64;
  const unsigned int inflights = max_recvs / 2;
  struct ibv_wc wc[max_recvs];
  unsigned int post_recvs = 0, poll_recvs = 0;

  size_t req_size = sizeof(struct req) + batch_size;
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
        {
          // print_payload((char *) sbuf + r->addr, r->size);
          wait_until_us(net_lat);
          send_async((char *) sbuf + r->addr, r->size);
        }
        else
        {
          // recv playload
          // print_payload(r+1, r->size);
          wait_until_us(net_lat);
          sbuf_populate(r->addr, r->size, r+1);
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