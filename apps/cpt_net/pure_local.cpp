#include <iostream>
#include <chrono>
#include <numeric>
// #include "clock.hpp"
// #include "cycles.h"

constexpr static uint64_t kNumEntries = 32 << 20;
constexpr static uint64_t per_batch = (8 << 20) / 8;
constexpr static uint64_t num_batch = kNumEntries / per_batch;
static uint64_t sum = 0;

void flush(uint64_t *fbuf)
{
  for (uint64_t i = 0; i < kNumEntries; ++ i)
  {
    fbuf[i] = i;
  }
}

template<uint64_t len>
uint64_t accum(uint64_t *lats)
{
  uint64_t rel = 0;
  for (uint64_t i = 0; i < len; ++ i)
  {
    rel += lats[i];
  }
  return rel;
}

using namespace std;
using namespace std::chrono;
int main()
{ 
  uint64_t *ary = (uint64_t *) malloc(sizeof(uint64_t) * kNumEntries);

  for (uint64_t i = 0; i < kNumEntries; ++ i)
  {
    ary[i] = i;
  }
  uint64_t *blats = (uint64_t *) malloc(sizeof(uint64_t) * num_batch);
  uint64_t *fbuf = (uint64_t *) malloc(sizeof(uint64_t) * kNumEntries);
  // flush(fbuf);
  for (uint64_t i = 0; i < num_batch; ++i)
  {
    auto bstart = chrono::high_resolution_clock::now();
    for (uint64_t j = 0; j < per_batch; ++ j)
    {
      // sum += ary[kNumEntries - (i * per_batch + j) - 1];
      sum += ary[i * per_batch + j];
      // sum &= ary[j];
      // stop_watch<chrono::nanoseconds>(0);
      // wait_until_cycles(2);
    }
    auto bend = chrono::high_resolution_clock::now();
    blats[i] = chrono::duration_cast<nanoseconds>(bend - bstart).count();
  }
  cout << "sum: " << sum << ", ns: " << accum<num_batch>(blats) << endl;
  for (uint64_t i = 0; i < num_batch; ++i)
  {
    cout << blats[i] << " ";
  }
  cout << endl;
  return 0;
}