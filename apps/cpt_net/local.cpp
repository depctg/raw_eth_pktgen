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

constexpr static uint64_t kNumEntries = 64 << 20;
constexpr static uint64_t batch_size = 8 << 10;
constexpr static int iter_ahead = 30;
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

static inline void pre_ahead(struct req *r, uint64_t *wr_ids)
{
  for (uint8_t i = 0; i < iter_ahead; ++ i)
  {
    r[i].addr = i * batch_size;
    r[i].size = batch_size;
    r[i].type = 1;
    send_async(r + i, sizeof(struct req));
    wr_ids[i] = recv_async((char *) rbuf + i * batch_size, batch_size);
  }
}

using namespace std;

int main(int argc, char ** argv)
{
  init(TRANS_TYPE_RC, argv[1]);
  const int num_buf = 256;
  uint64_t *wr_ids = (uint64_t *) malloc(sizeof(uint64_t) * num_buf);
  struct req *reqs = (struct req *) sbuf;
  recv(rbuf, sizeof(struct req));
  // reqs[0].addr = 0;
  // reqs[0].size = batch_size;
  // reqs[0].type = 1;
  // send_async(reqs, sizeof(struct req));
  // wr_ids[0] = recv_async(rbuf, batch_size);
  auto start = chrono::steady_clock::now();
  pre_ahead(reqs, wr_ids);
  int buf_id = iter_ahead - 1;
  int read_id = 0;
  uint64_t steady_loop = kNumEntries - per_batch * iter_ahead;
  for (uint64_t i = 0; i < steady_loop; i += per_batch)
  {
    // prefetch
    int buf_id_nxt = (buf_id + 1) % num_buf;
    reqs[buf_id_nxt].addr = i * sizeof(uint64_t) + iter_ahead * batch_size;
    reqs[buf_id_nxt].size = batch_size;
    reqs[buf_id_nxt].type = 1;
    send_async(reqs + buf_id_nxt, sizeof(struct req));
    wr_ids[buf_id_nxt] = recv_async((uint64_t *) rbuf + buf_id_nxt * per_batch, batch_size);

    // process
    poll(wr_ids[read_id]);
    uint64_t *ary = (uint64_t *) rbuf + read_id * per_batch;
    for (uint64_t j = 0; j < per_batch; ++ j)
      do_sth(ary + j);
    read_id = (read_id + 1) % num_buf;
    buf_id = buf_id_nxt;
  }
  for (uint64_t i = steady_loop; i < kNumEntries; i+= per_batch)
  {
    poll(wr_ids[read_id]);
    uint64_t *ary = (uint64_t *) rbuf + read_id * per_batch;
    for (uint64_t j = 0; j < per_batch; ++j)
      do_sth(ary + j);
    read_id = (read_id + 1) % num_buf;
  }
  auto end = chrono::steady_clock::now();
  cout << "sum: " << sum << ", ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
}
