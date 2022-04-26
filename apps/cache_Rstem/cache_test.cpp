#include <iostream>
#include <chrono>
#include "common.h"
#include "greeting.h"
#include "cache.h"
#include "patterns.hpp"
#include <assert.h>

constexpr static uint64_t max_size = 64 << 20;
constexpr static uint32_t cache_line_size = 8192;
constexpr static uint32_t array_size = 8 << 20;
constexpr static uint64_t num_access_times = 4 << 20;
constexpr static uint64_t tile = 2048;
constexpr static uint64_t sigma = 8388608;
constexpr static double skewness = 0.0;

uint64_t mutate = 0;

using namespace std;
int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);
    CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf);

    // vector<size_t> access_pattern = gen_access_pattern_normal(num_access_times, array_size / 2, 1, sigma, 2333);
    // vector<size_t> access_pattern = gen_access_pattern_uniform(num_access_times, array_size, tile, 2333);
    vector<size_t> access_pattern = gen_access_pattern_zipf(num_access_times, array_size, skewness, 1, 10);
    recv((char *)rbuf + max_size, 1);
    auto start = chrono::steady_clock::now();

    // random access
    for (size_t i = 0; i < num_access_times; ++i)
    {
        char *l = cache_access(cache, access_pattern[i] * sizeof(uint64_t));
        uint64_t v = *((uint64_t *) l); 
        mutate ^= v;
        // cout << v << endl << access_pattern[i] << endl;
        assert(v == access_pattern[i]);
    }
    
    // sequential complete access
    // for (size_t i = 0; i < array_size; ++i)
    // {
    //     char *l = cache_access(cache, i * sizeof(uint64_t));
    //     uint64_t v = *((uint64_t *) l); 
    //     // cout << (void *) l << endl;
    //     mutate ^= v;
    //     // assert(v == i);
    //     cout << v << endl;
    // }

    auto end = chrono::steady_clock::now();
    std::cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << ", miss rate: " << ((float)cache->misses / (float)cache->accesses) * 100 << std::endl;
    return 1;
}
