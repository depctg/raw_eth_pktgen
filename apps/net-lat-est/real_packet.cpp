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

constexpr static uint64_t batch_size = 16 << 20;
constexpr static uint64_t iterations = 128;

using namespace std;
int main(int argc, char **argv)
{
  init(TRANS_TYPE_RC, argv[1]);
  const int num_buf = 256;
  // uint64_t *wr_ids = (uint64_t *) malloc(sizeof(uint64_t) * num_buf);
  struct req *reqs = (struct req *) sbuf;
  // pre_ahead(reqs, wr_ids);
  uint64_t total_hw = 0;
  uint64_t total_sw = 0;
  uint64_t *lats = (uint64_t *) malloc(sizeof(uint64_t) * iterations);
  for (uint64_t i = 0; i < iterations; ++ i)
  {
    auto sw_start = chrono::high_resolution_clock::now();
    uint64_t idx = i % num_buf;
    reqs[idx].addr = idx * batch_size;
    reqs[idx].size = batch_size;
    reqs[idx].type = 1;
    // send_async(reqs + idx, sizeof(struct req));
    // uint64_t wr_id = recv_async((char *)rbuf + idx * batch_size, batch_size);
    auto sw_end = chrono::high_resolution_clock::now();

    send(reqs + idx, sizeof(struct req));
    recv((char *)rbuf + idx * batch_size, batch_size);
    // poll(wr_id);
    auto hw_end = chrono::high_resolution_clock::now();
    total_sw += chrono::duration_cast<nanoseconds>(sw_end - sw_start).count();
    uint64_t hwi = chrono::duration_cast<nanoseconds>(hw_end - sw_end).count();
    total_hw += hwi;
    lats[i] = hwi / (double) 1000;
  }

  sort(lats, lats + iterations);
  cout << "hw: " << total_hw / 1000 << ", sw: " << total_sw / 1000 << ", median: " << (lats[iterations / 2] + lats[(iterations -1) /2]) / (double) 2 << endl;
  // ofstream packlog ("netlog");
  // for (int i = 0; i < iterations; ++i)
  // {
  //   packlog << lats[i] << endl;
  // }
  // packlog.close();
  return 0;
}