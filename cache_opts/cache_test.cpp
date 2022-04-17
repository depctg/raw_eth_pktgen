#include <iostream>
#include <chrono>
#include "../common.h"
#include "../greeting.h"
#include "../cache.h"
#include "../patterns.h"
#include <assert.h>

constexpr static uint64_t max_size = 16 << 20;
constexpr static uint32_t cache_line_size = 1024;
// constexpr static uint32_t array_size = 4 << 20;
// constexpr static uint64_t num_access_times = 8 << 20;
// constexpr static uint64_t sigma = array_size / 4;
constexpr static uint32_t array_size = 4 << 20;
constexpr static uint64_t num_access_times = array_size;

uint64_t mutate = 0;

using namespace std;
int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);
    CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf);

    // vector<size_t> access_pattern = gen_access_pattern_normal(num_access_times, array_size / 2, 1, sigma, 2333);
    // vector<size_t> access_pattern = gen_access_pattern_uniform(num_access_times, array_size, 1, 2333);
    recv((char *)rbuf + max_size, 1);
    auto start = chrono::steady_clock::now();

    // random access
    // for (size_t i = 0; i < num_access_times; ++i)
    // {
    //     char *v = cache_access(cache, access_pattern[i] * sizeof(uint64_t));
    //     assert(*((uint64_t *) v) == access_pattern[i]);
    //     // cout << *((uint64_t*) v) << endl;
    // }
    
    // sequential complete access
    for (size_t i = 0; i < array_size; ++i)
    {
        char *l = cache_access(cache, i * sizeof(uint64_t));
        uint64_t v = *((uint64_t *) l); 
        mutate ^= v;
        assert(v == i);
        // cout << *((uint64_t*) v) << endl;
    }

    auto end = chrono::steady_clock::now();
    std::cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << ", miss rate: " << ((float)cache->misses / (float)num_access_times) * 100 << std::endl;
    return 1;
}
