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

constexpr static uint64_t kNumEntries = 32 << 20;
constexpr static uint64_t batch_size = 512;
constexpr static int iter_ahead = 0;
constexpr static uint64_t per_batch = batch_size / sizeof(uint64_t);
constexpr static uint64_t num_batch = kNumEntries / per_batch;
static uint64_t rel = 0;

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
  rel += *ele;
  // rel &= *ele;
}

static inline void pre_ahead(struct req *r, uint64_t *wr_ids)
{
  for (uint8_t i = 0; i < iter_ahead; ++ i)
  {
    r[i].addr = i * batch_size;
    r[i].size = batch_size;
    r[i].type = 1;
    // send_async(r + i, sizeof(struct req));
    // wr_ids[i] = recv_async((char *) rbuf + i * batch_size, batch_size);
    send(r + i, sizeof(struct req));
    wr_ids[i] = recv((char *) rbuf + i * batch_size, batch_size);
  }
}

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv)
{
  init(TRANS_TYPE_RC, argv[1]);

  uint64_t total_comp = 0;
  uint64_t total_swof = 0;
  uint64_t hw_p0 = 0;
  uint64_t hw_p1 = 0;
  uint64_t hw_p2 = 0;

  const int num_buf = 64;
  uint64_t *wr_ids = (uint64_t *) malloc(sizeof(uint64_t) * num_buf);
  struct req *reqs = (struct req *) sbuf;
  recv(rbuf, sizeof(struct req));

  auto start = steady_clock::now();

  auto p0_start = chrono::high_resolution_clock::now();
  pre_ahead(reqs, wr_ids);
  auto p0_end = chrono::high_resolution_clock::now();
  hw_p0 = chrono::duration_cast<nanoseconds>(p0_end - p0_start).count();
  int buf_id = iter_ahead - 1;
  int read_id = 0;
  uint64_t pf_loops = num_batch - iter_ahead;

  for (uint64_t i = 0; i < pf_loops; ++i)
  {
    // prefetch
    auto sw_start = high_resolution_clock::now();
    int buf_id_nxt = (buf_id + 1) % num_buf;
    reqs[buf_id_nxt].addr = (i + iter_ahead) * batch_size;
    reqs[buf_id_nxt].size = batch_size;
    reqs[buf_id_nxt].type = 1;
    send_async(reqs + buf_id_nxt, sizeof(struct req));
    wr_ids[buf_id_nxt] = recv_async((uint64_t *) rbuf + buf_id_nxt * per_batch, batch_size);
    auto sw_end = high_resolution_clock::now();
    total_swof += duration_cast<nanoseconds>(sw_end - sw_start).count();

    // cout << wr_ids[buf_id_nxt] << endl;

    // process
    auto hw_start = high_resolution_clock::now();
    poll(wr_ids[read_id]);
    auto hw_end = high_resolution_clock::now();
    uint64_t hwspan = duration_cast<nanoseconds>(hw_end - hw_start).count();
    hw_p1 += hwspan;

    uint64_t *ary = (uint64_t *) rbuf + read_id * per_batch;
    auto comp_start = high_resolution_clock::now();
    for (uint64_t j = 0; j < per_batch; ++ j)
    {
      rel += ary[j];
      // cout << ary[j] << endl;
      // do_sth(ary + j);
    }
    auto comp_end = high_resolution_clock::now();
    total_comp += duration_cast<nanoseconds>(comp_end-comp_start).count();
    read_id = (read_id + 1) % num_buf;
    buf_id = buf_id_nxt;
  }
  for (uint64_t i = 0; i < iter_ahead; ++i)
  {
    auto hw_start = high_resolution_clock::now();
    poll(wr_ids[read_id]);
    auto hw_end = high_resolution_clock::now();
    hw_p2 += duration_cast<nanoseconds>(hw_end - hw_start).count();
    
    uint64_t *ary = (uint64_t *) rbuf + read_id * per_batch;
    auto comp_start = high_resolution_clock::now();
    for (uint64_t j = 0; j < per_batch; ++ j)
      rel += ary[j];
      // do_sth(ary + j);
    auto comp_end = high_resolution_clock::now();
    total_comp += duration_cast<nanoseconds>(comp_end-comp_start).count();
    read_id = (read_id + 1) % num_buf;
  }
  auto end = chrono::steady_clock::now();
  cout << "comp: " << total_comp / double(1000) << endl
  << "sw: " <<  total_swof/ double(1000)  << endl
  << "hw_p0: " << hw_p0 / double(1000) << ", avg: " << hw_p0 / double(1000 * iter_ahead) << endl
  << "hw_p1: " << hw_p1 / double(1000) << ", avg: " << hw_p1 / double(1000 * (num_batch - iter_ahead)) << endl
  << "hw_p2: " << hw_p2 / double(1000) << ", avg: " << hw_p2 / double(1000 * iter_ahead) << endl;
  cout << "rel: " << rel << ", ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
}
