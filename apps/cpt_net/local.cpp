#include <iostream>
#include <chrono>
#include "common.h"
#include "greeting.h"
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <memory>
#include <random>
#include "clock.hpp"

constexpr static uint64_t kNumEntries = (4 << 20);
constexpr static uint64_t batch_size = (4096);
constexpr static uint64_t per_batch = batch_size / sizeof(uint64_t);
static uint64_t sum = 0;

static std::chrono::time_point<std::chrono::steady_clock> timebase;

void synctime_send() {
    auto t0 = chrono::steady_clock::now();
    send(sbuf, sizeof(req));
    recv(rbuf, sizeof(req));
    auto t1 = chrono::steady_clock::now();
    timebase = t0 + (t1 - t0) / 2;
}

static inline void set_timestamp(struct req * r) {
    auto duration = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now() - timebase).count();
    r->timestamp = duration;
}

static void do_sth(uint64_t *ele)
{
  sum += *ele;
}

using namespace std;

int main(int argc, char ** argv)
{
  init(TRANS_TYPE_RC, argv[1]);
  
  uint64_t wr_id, wr_id_nxt;
  struct req *reqs = (struct req *) sbuf;

  const int num_buf = 512;
  int buf_id = 0;

  auto start = chrono::steady_clock::now();
  reqs[buf_id].addr = 0;
  reqs[buf_id].size = batch_size;
  reqs[buf_id].type = 1;
  send_async(reqs + buf_id, sizeof(struct req));
  wr_id = recv_async(rbuf, batch_size);

  for (uint64_t i = 0; i < kNumEntries; i += per_batch)
  {
    // prefetch
    int buf_id_nxt = (buf_id + 1) % num_buf;
    if (i + per_batch < kNumEntries)
    {
      reqs[buf_id_nxt].addr = i * sizeof(uint64_t) + batch_size;
      reqs[buf_id_nxt].size = batch_size;
      reqs[buf_id_nxt].type = 1;
      send_async(reqs + buf_id_nxt, sizeof(struct req));
      wr_id_nxt = recv_async((uint64_t *) rbuf + buf_id_nxt * per_batch, batch_size);
    }

    poll(wr_id);
    uint64_t *ary = (uint64_t *) rbuf + buf_id * per_batch;
    for (uint64_t j = 0; j < per_batch; ++ j)
      do_sth(ary + j);
    wr_id = wr_id_nxt;
    buf_id = buf_id_nxt;
  }
  auto end = chrono::steady_clock::now();
  cout << "sum: " << sum << ", ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
}
