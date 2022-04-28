#include <iostream>
#include <chrono>
#include "common.h"
#include "greeting.h"
#include <memory>

constexpr static uint64_t packet_size = 128 << 10;
constexpr static uint64_t num_iter = 10000;

uint64_t find_min(uint64_t *ary, int length)
{
  uint64_t rel = *ary;
  for (int i = 1; i < length; ++i)
    rel = rel < *(ary + i) ? rel : *(ary+i);
  return rel;
}

using namespace std;
using namespace std::chrono;
int main(int argc, char **argv)
{
  init(TRANS_TYPE_RC, argv[1]);
  uint64_t total_lat = 0;
  uint64_t *lats = (uint64_t *) malloc(num_iter * sizeof(uint64_t));
  for (uint64_t i = 0; i < num_iter; ++i)
  {
    auto start = high_resolution_clock::now();
    send((char *) sbuf + i * packet_size, packet_size);
    recv(rbuf, packet_size);
    auto end = high_resolution_clock::now();
    *(lats + i) = duration_cast<microseconds>(end - start).count(); 
    total_lat += *(lats + i);
  }
  cout << "size | min | avg" << endl << packet_size << " | " << find_min(lats, num_iter) / (double) 2 << " | " << (double) total_lat / (2*num_iter) << endl;
}