#include <iostream>
#include <chrono>
#include "common.h"
#include "greeting.h"
#include "cache.h"
#include "patterns.hpp"
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <memory>
#include <random>

constexpr static uint64_t kCacheSize = 128 << 20;
constexpr static uint64_t kNumEntries = (8ULL << 20);
constexpr static uint64_t kCacheLineSize = (2048);
constexpr static double skewness = 0;
constexpr static int tiles = 256;

constexpr static int num_lines = kNumEntries * sizeof(uint64_t) / kCacheLineSize;

uint64_t mut = 1010;

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

void cacheAry(uint64_t *ary, uint64_t offset, CacheTable *cache)
{
  for (int i = 0; i < num_lines; ++i)
  {
    uint64_t tag = ((i * kCacheLineSize + offset) & cache->addr_mask) >> cache->tag_shifts;
    cache_insert(cache, tag, (char *) ary + i*kCacheLineSize);
  }
}

void evictAry(uint64_t *ary, uint64_t offset, CacheTable *cache)
{
  for (int i = 0; i < num_lines; ++i)
  {
    uint64_t tag = ((i * kCacheLineSize + offset) & cache->addr_mask) >> cache->tag_shifts;
    remote_write(cache, tag, (char *) ary + i * kCacheLineSize);
  }
}
using namespace std;
int main(int argc, char * argv[])
{
  init(TRANS_TYPE_RC, argv[1]);
  CacheTable *cache = createCacheTable(kCacheSize - sizeof(uint64_t) * kNumEntries, kCacheLineSize, sbuf, rbuf);

  uint64_t *A = (uint64_t *) malloc(kNumEntries * sizeof(uint64_t));
  uint64_t *B = (uint64_t *) malloc(kNumEntries * sizeof(uint64_t));
  uint64_t *C = (uint64_t *) malloc(kNumEntries * sizeof(uint64_t));
  prepareAry(A); prepareAry(B);
  uint64_t b_offset = kNumEntries * sizeof(uint64_t);
  uint64_t c_offset = 2 * kNumEntries * sizeof(uint64_t);

  auto start = chrono::steady_clock::now();
  // write A to remote
  evictAry(A, 0, cache);
  // Write B to remote
  evictAry(B, b_offset, cache);
  cout << "all remote" << endl;

  vector<size_t> access_pattern = gen_access_pattern_zipf(4 << 20, kNumEntries, skewness, tiles, 1000);
  // vector<size_t> access_pattern = gen_access_pattern_seq(kNumEntries, kNumEntries);
  for (auto i : access_pattern)
  {
    uint64_t *ai = (uint64_t *) cache_access(cache, i * sizeof(uint64_t));
    uint64_t avi = *ai;
    uint64_t *bi = (uint64_t *) cache_access(cache, b_offset + i * sizeof(uint64_t));
    uint64_t bvi = *bi;
    mut ^= (avi + bvi);
    // cout << i << endl;
    // cache_write(cache, c_offset + i * sizeof(uint64_t), &mut);
    C[i] = mut;
  }
  evictAry(C, c_offset, cache);
  auto end = chrono::steady_clock::now();
  std::cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << ", miss rate: " << ((float)cache->misses / (float)cache->accesses) * 100 << std::endl;

  return 1;
}