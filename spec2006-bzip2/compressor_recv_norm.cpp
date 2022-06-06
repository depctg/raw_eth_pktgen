#include <iostream>
#include "common.h"
#include "greeting.h"
#include <assert.h>
#include <string>
#include "clock.hpp"

using namespace std;

constexpr static uint64_t batch_size = 1 << 20;
constexpr static uint64_t net_lat = 0; /*us*/

static std::chrono::time_point<std::chrono::steady_clock> timebase;

void synctime_recv() {
    recv(rbuf, sizeof(req));
    timebase = chrono::steady_clock::now();
    send(sbuf, sizeof(req));
}

static inline auto get_target_timestamp(struct req * r, duration_t extra_delay) {
    chrono::nanoseconds diff{r->timestamp}, extra{extra_delay};
    return timebase + diff + extra;
}

int main(int argc, char **argv)
{
  init(TRANS_TYPE_RC_SERVER, argv[1]);
  cout << "Start processing requests" << endl;
  const unsigned int max_recvs = 64;
  const unsigned int inflights = max_recvs / 2;
  struct ibv_wc wc[max_recvs];
  unsigned int post_recvs = 0, poll_recvs = 0;

  char *req_recv_buf = (char *) rbuf;
  char *data_buf = (char *) sbuf + (256 << 20);

  uint64_t recv_size = sizeof(struct req) + batch_size;
  for (unsigned int i = 0; i < inflights; i++)
    recv_async(req_recv_buf + i * recv_size, recv_size);
  post_recvs += inflights;

  while (1)
  {
    int n = poll_cq(cq, MAX_POLL, wc);
    for (int i = 0; i < n; ++i)
    {
      if (wc[i].status == 0 && wc[i].opcode == IBV_WC_RECV)
      {
        int idx = (poll_recvs ++) % max_recvs;
        struct req *r = (struct req *) (req_recv_buf + idx * recv_size);
        // const char *type = r->type == 1 ? "Fetch" : "Update";
        // cout << "Req " << r->addr << ", size " << r->size << ", type " << type << endl;
        if (r->type)
          send_async(data_buf + r->addr, r->size);
        else
          memcpy(data_buf + r->addr, r+1, r->size);
      }
    }
    while (post_recvs < poll_recvs + inflights)
    {
      int idx = (post_recvs ++) % max_recvs;
      recv_async(req_recv_buf + idx * recv_size, recv_size);
    }
  }
}
