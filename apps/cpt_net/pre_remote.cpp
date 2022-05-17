#include <iostream>
#include "common.h"
#include "greeting.h"
#include <assert.h>
#include <string>
#include "clock.hpp"

using namespace std;

constexpr static uint64_t kNumEntries = 32 << 20;
constexpr static uint64_t batch_size = 512;
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

static inline void app_init()
{
  uint64_t sum = 0;
  uint64_t *ary = (uint64_t *) sbuf;
  for (uint64_t i = 0; i < kNumEntries; ++ i)
  {
    sum += i;
    *(ary + i) = i;
  }
  cout << "Remote sum: " << sum << endl;
}

int main(int argc, char **argv)
{
  init(TRANS_TYPE_RC_SERVER, argv[1]);
  app_init();
  cout << "Start processing requests" << endl;
  const unsigned int max_recvs = 64;
  const unsigned int inflights = max_recvs / 2;
  struct ibv_wc wc[max_recvs];
  unsigned int post_recvs = 0, poll_recvs = 0;

  struct req *reqs = (struct req *) rbuf;

  // notify
  send(sbuf, sizeof(struct req));

  for (int i = 0; i < inflights; i++)
    recv_async(reqs + i, sizeof(struct req));
  post_recvs += inflights;

  while (1)
  {
    int n = poll_cq(cq, inflights, wc);
    for (int i = 0; i < n; ++i)
    {
      if (wc[i].status == 0 && wc[i].wr_id != 0)
      {
        int idx = (poll_recvs ++) % max_recvs;
        struct req *r = reqs + idx;
        // const char *type = r->type == 1 ? "Fetch" : "Update";
        // cout << "Req " << r->addr << ", size " << r->size << ", type " << type << endl;
        send_async((char *) sbuf + r->addr, r->size);
      }
    }
    while (post_recvs < poll_recvs + inflights)
    {
      int idx = (post_recvs ++) % max_recvs;
      recv_async(reqs + idx, sizeof(struct req));
    }
  }
}
