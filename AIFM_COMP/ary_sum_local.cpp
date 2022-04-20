#include <iostream>
#include <chrono>
#include "../common.h"
#include "../greeting.h"
#include "../cache.h"
#include "../patterns.h"
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <memory>
#include <random>

constexpr static uint64_t kCacheSize = 480 << 20;
constexpr static uint64_t kNumEntries = (16ULL << 20);
constexpr static uint64_t kCacheLineSize = (16384 * 2);

constexpr static int num_lines = kNumEntries * sizeof(uint64_t) / kCacheLineSize;

void prepareAry(uint64_t *ary)
{
  std::random_device rd;
  std::mt19937_64 eng(rd());
  std::uniform_int_distribution<uint64_t> distr;
  for (int i = 0; i < kNumEntries; ++i)
  {
    ary[i] = distr(eng);
    // ary[i] = i;
  }
}

void evictAry(uint64_t *ary, uint64_t offset, CacheTable *cache)
{
  for (int i = 0; i < num_lines; ++i)
  {
    uint64_t tag = ((i * kCacheLineSize + offset) & cache->addr_mask) >> cache->tag_shifts;
    // remote_write(cache, tag, (char *) ary + i * kCacheLineSize);
    cache_insert(cache, tag, (char *) ary + i*kCacheLineSize);
  }
}

using namespace std;
int main(int argc, char * argv[])
{
  init(TRANS_TYPE_RC, argv[1]);
  CacheTable *cache = createCacheTable(kCacheSize, kCacheLineSize, sbuf, rbuf);

  uint64_t *A = (uint64_t *) malloc(kNumEntries * sizeof(uint64_t));
  uint64_t *B = (uint64_t *) malloc(kNumEntries * sizeof(uint64_t));
  prepareAry(A); prepareAry(B);
  recv((char *)rbuf + kCacheSize + sizeof(uint64_t), 1);
  auto start = chrono::steady_clock::now();
  // write A to remote
  evictAry(A, 0, cache);
  // Write B to remote
  evictAry(B, kNumEntries * sizeof(uint64_t), cache);
  uint64_t b_offset = kNumEntries * sizeof(uint64_t);
  uint64_t c_offset = 2 * kNumEntries * sizeof(uint64_t);
  for (int i = 0; i < kNumEntries; ++i)
  {
    uint64_t *ai = (uint64_t *) cache_access(cache, i * sizeof(uint64_t));
    uint64_t avi = *ai;
    uint64_t *bi = (uint64_t *) cache_access(cache, b_offset + i * sizeof(uint64_t));
    uint64_t bvi = *bi;
    uint64_t ci = avi + bvi;
    // cout << ci << endl;
    assert(ci == A[i] + B[i]);
    cache_write(cache, c_offset + i * sizeof(uint64_t), &ci);
  }
  auto end = chrono::steady_clock::now();
  std::cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << ", miss rate: " << ((float)cache->misses / (float)cache->accesses) * 100 << std::endl;

  // for (int i = 0; i < kNumEntries; ++i)
  // {
  //   uint64_t * ci = (uint64_t *) cache_access(cache, c_offset + i * sizeof(uint64_t));
  //   assert(*ci == A[i] + B[i]);
  //   // cout << *ci << endl;
  // }
  return 1;
}