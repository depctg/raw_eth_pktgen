#include <iostream>
#include <chrono>
#include "common.h"
#include "greeting.h"
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <memory>
#include "clock.hpp"
#include <cstdlib>
#include <fstream>

constexpr static uint64_t batch_size = 4 << 10;
constexpr static int iter_ahead = 1;
constexpr static uint64_t iterations = 64;

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

static void do_sth(void *buf)
{
  stop_watch<chrono::microseconds>(1);
}

using namespace std;
int main(int argc, char **argv)
{
  init(TRANS_TYPE_RC, argv[1]);

  uint64_t total_comp = 0;
  uint64_t total_swof = 0;
  uint64_t hw_first_ahead = 0;
  uint64_t hw_steady = 0;
  uint64_t hw_last = 0;
  uint64_t *hwi = (uint64_t *) malloc(sizeof(uint64_t) * iterations);

  const int num_buf = 32;
  uint64_t *wr_ids = (uint64_t *) malloc(sizeof(uint64_t) * num_buf);
  struct req *reqs = (struct req *) sbuf;

  for (int i = 0; i < 100; ++i)
  {
    send_async(reqs, sizeof(struct req));
    uint64_t id = recv_async(rbuf, batch_size);
    poll(id);
  }

  auto start = chrono::steady_clock::now();
  auto p0_start = chrono::high_resolution_clock::now();
  pre_ahead(reqs, wr_ids);
  auto p0_end = chrono::high_resolution_clock::now();
  hw_first_ahead = chrono::duration_cast<nanoseconds>(p0_end - p0_start).count();
  int buf_id = iter_ahead - 1;
  int read_id = 0;
  uint64_t steady_loop = iterations - iter_ahead;

  // ith iteration
  for (uint64_t i = 0; i < steady_loop; i ++ )
  {
    // prefetch
    auto sw_start = high_resolution_clock::now();
    int buf_id_nxt = (buf_id + 1) % num_buf;
    reqs[buf_id_nxt].addr = (i + iter_ahead) * batch_size;
    reqs[buf_id_nxt].size = batch_size;
    reqs[buf_id_nxt].type = 1;
    send_async(reqs + buf_id_nxt, sizeof(struct req));
    wr_ids[buf_id_nxt] = recv_async((char *) rbuf + buf_id_nxt * batch_size, batch_size);
    auto sw_end = high_resolution_clock::now();
    total_swof += duration_cast<nanoseconds>(sw_end - sw_start).count();

    // process current batch
    auto hw_start = chrono::high_resolution_clock::now();
    poll(wr_ids[read_id]);
    auto hw_end = chrono::high_resolution_clock::now();
    uint64_t hwspan = duration_cast<nanoseconds>(hw_end - hw_start).count();
    hw_steady += hwspan;
    hwi[i] = hwspan;
    // uint64_t *f = (uint64_t *) ((char *) rbuf + read_id * batch_size);
    // cout << *f << endl;

    auto comp_start = high_resolution_clock::now();
    /*do something*/
    do_sth((char *) rbuf + read_id * batch_size);
    auto comp_end = high_resolution_clock::now();
    total_comp += duration_cast<nanoseconds>(comp_end-comp_start).count();
    read_id = (read_id + 1) % num_buf;
    buf_id = buf_id_nxt;
  }

  for (uint64_t i = steady_loop; i < iterations; i++)
  {
    auto hw_start = high_resolution_clock::now();
    poll(wr_ids[read_id]);
    auto hw_end = high_resolution_clock::now();
    uint64_t hwspan = duration_cast<nanoseconds>(hw_end - hw_start).count();
    hw_last += hwspan;
    hwi[i] = hwspan;

    auto comp_start = high_resolution_clock::now();
    /*do something*/
    do_sth((char *) rbuf + read_id * batch_size);
    auto comp_end = high_resolution_clock::now();
    total_comp += duration_cast<nanoseconds>(comp_end-comp_start).count();
    read_id = (read_id + 1) % num_buf;
  }
  auto end = chrono::steady_clock::now();
  cout << "comp: " << total_comp / double(1000) << endl
  << "sw: " <<  total_swof/ double(1000)  << endl
  << "hw_p0: " << hw_first_ahead / double(1000) << endl
  << "hw_p1: " << hw_steady/ double(1000) << endl
  << "hw_p2: " << hw_last / double(1000) << endl;
  cout << "total ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;

  cout << "Poll lat: " << endl;
  for (uint64_t i = 0; i < iterations; ++ i)
  {
    cout << i << ": " << hwi[i] << endl;
  }
  return 0;
}