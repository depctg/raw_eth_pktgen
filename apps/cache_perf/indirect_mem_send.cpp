#include <iostream>
#include <chrono>
#include "common.h"
#include "greeting.h"
#include "cache.h"
#include "replacement.h"
#include "patterns.hpp"
#include "clock.hpp"
#include <assert.h>

constexpr static uint64_t max_size = 1 << 10;
constexpr static uint64_t cache_line_size = 1 << 7;
constexpr static uint64_t work_size = 1 << 10;
constexpr static uint64_t tile = 1;
constexpr static uint64_t seed = 2333;
constexpr static uint64_t future_depth = work_size;

void do_sth(uint64_t i)
{
    stop_watch<chrono::microseconds>(5);
}

using namespace std;
int main(int argc, char * argv[]) {
  vector<uint64_t> t_vec = gen_access_pattern_uniform(work_size, work_size, tile, seed);
	init(TRANS_TYPE_RC, argv[1]);
  // CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf, future_depth, add_to_OPT, touch_OPT, pop_OPT);
  CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf, 0, add_to_LRU, touch_LRU, pop_LRU);
  // CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf, work_size, add_to_LFU, touch_LFU, pop_LFU);

  // write to cache
  uint64_t *t_ary = (uint64_t *) malloc(sizeof(uint64_t) * work_size);
  for (uint64_t i = 0; i < work_size; ++ i) {
    t_ary[i] = t_vec[i];
    // cout << t_ary[i] << " ";
  }
  // cout << endl;

  remote_write_n(cache, 0, t_ary, sizeof(uint64_t) * work_size);

  /* create future refs */
  uint64_t future_tags[work_size] = {0};
  for (uint64_t i = 1; i < work_size; ++ i) {
    future_tags[i] = t_ary[future_tags[i-1]];
  }
  for (uint64_t i = 1; i < work_size; ++ i) {
    future_tags[i] = future_tags[i] * sizeof(uint64_t) >> cache->tag_shifts;
  }

  // for (uint64_t i = 0; i < 15; ++i)
  //   cout << future_tags[i] << " ";
  // cout << endl;

  // regist_future(cache->rplc, future_tags, work_size, max_size / cache_line_size);
  // display_future(cache->rplc);

  auto start = chrono::steady_clock::now();
  uint64_t prev_idx = 0;
  // prefetch(cache, prev_idx);
  for (uint64_t i = 0; i < work_size; ++ i) {
    // if (i + 1 < work_size)
    //   prefetch(cache, t_ary[prev_idx] * sizeof(uint64_t));
    uint64_t v = *(uint64_t *) cache_access(cache, prev_idx * sizeof(uint64_t));
    // cout << i << " " << v << endl;
    assert(v == t_ary[prev_idx]);
    do_sth(v);
    prev_idx = v;
  }

  auto end = chrono::steady_clock::now();
  std::cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << ", miss rate: " << ((float)cache->misses / (float)cache->accesses) * 100 << std::endl;
  return 1;
}
