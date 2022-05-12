#include <iostream>
#include <chrono>
#include "common.h"
#include "greeting.h"
#include <memory>
#include <cstdlib>

constexpr static uint64_t packet_size = 8 << 20;
constexpr static uint64_t num_iter = 100;

using namespace std;
using namespace std::chrono;
int main(int argc, char **argv)
{
  init(TRANS_TYPE_RC, argv[1]);
  uint64_t total_lat = 0;
  uint64_t *lats = (uint64_t *) malloc(num_iter * sizeof(uint64_t));
  const uint64_t num_buf = 256;
  struct req *reqs = (struct req *) sbuf;

  // pre-heat
  for (uint64_t i = 0; i < num_iter; ++i)
  {
    send_async(sbuf, packet_size);
    uint64_t id = recv_async(rbuf, packet_size);
    poll(id);
  }

  // real estimation
  for (uint64_t i = 0; i < num_iter; ++i)
  {
    uint64_t idx = i % num_buf;
    auto start = high_resolution_clock::now();
    send((char *) sbuf + idx * packet_size, packet_size);
    // send(reqs + idx, sizeof(struct req));
    recv(rbuf, packet_size);
    auto end = high_resolution_clock::now();
    *(lats + i) = duration_cast<microseconds>(end - start).count(); 
    total_lat += *(lats + i);
  }
  sort(lats, lats + num_iter);
  cout << "size | min | median | mean | max " << endl 
  << packet_size << " | " 
  << lats[0] / (double) 2 << " | " 
  << ((num_iter % 2 == 0) ? (lats[num_iter / 2] + lats[(num_iter-1) / 2]) / 2 : lats[num_iter / 2]) / (double) 2 << " | "
  << (double) total_lat / (2*num_iter) << " | "
  << lats[num_iter - 1] / (double) 2 << endl;
}