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
constexpr static uint64_t array_size = 1 << 10;
constexpr static uint64_t cache_line_size = 1 << 8;
constexpr static uint64_t per_line = cache_line_size / (sizeof(uint64_t));
constexpr static uint64_t num_access_times = 1 << 20;
constexpr static uint64_t seed = 2333;
constexpr static uint64_t tile = 1;
constexpr static uint64_t sigma = 8388608;
constexpr static double skewness = 0.0;

void do_sth(void *i)
{
    stop_watch<chrono::microseconds>(5);
}

using namespace std;
int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);
    CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf, 0, add_to_LFU, touch_LFU, pop_LFU);

    // vector<size_t> access_pattern = gen_access_pattern_normal(num_access_times, array_size / 2, 1, sigma, 2333);
    // vector<size_t> access_pattern = gen_access_pattern_uniform(num_access_times, array_size, tile, seed);
    vector<size_t> access_pattern = gen_access_pattern_seq(num_access_times, array_size);
    // vector<size_t> access_pattern = gen_access_pattern_zipf(num_access_times, array_size, skewness, 1, 10);

    // for (size_t i : access_pattern)
    //     cout << i << endl;

    auto start = chrono::steady_clock::now();

    // prefetch(cache, 0);

    // random access
    for (size_t i = 0; i < num_access_times; i++ )
    {
        uint64_t addr = access_pattern[i] * sizeof(uint64_t);
        uint64_t tag = (addr & cache->addr_mask) >> cache->tag_shifts;
        uint64_t line_offset /* bytes */ = (addr & cache->tag_mask);
        // cout << i << "Access: " << addr << " tag " << tag << " ofst " << line_offset << endl;
        if (i % per_line == 0)
        {
            // prefetch(cache, ((addr & cache->addr_mask) + (1 << cache->tag_shifts)) % (array_size * sizeof(uint64_t)));
            // cout << i << " % " << num_access_times << endl;
        }
        uint64_t *l = (uint64_t *)cache_access(cache, addr);
        // cout << access_pattern[i] << " " << *l << " " << l << endl;
        do_sth(l);
        if (access_pattern[i] != *l)
            cout << access_pattern[i] << " " << *l << endl;
        // assert(access_pattern[i] == *l);
    }

    auto end = chrono::steady_clock::now();
    std::cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << ", miss rate: " << ((float)cache->misses / (float)cache->accesses) * 100 << std::endl;
    return 1;
}
