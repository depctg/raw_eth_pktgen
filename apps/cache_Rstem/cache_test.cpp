#include <iostream>
#include <chrono>
#include <assert.h>

#include "common.h"
#include "cache.h"
#include "patterns.hpp"
#include "clock.hpp"

constexpr static uint64_t cache_size = 1 << 7;
constexpr static uint64_t cache_line_size = 1 << 5;
constexpr static uint64_t per_line = cache_line_size / (sizeof(uint64_t));

constexpr static uint64_t array_size = 1 << 10;
constexpr static uint64_t num_access_times = 1 << 5;
constexpr static uint64_t seed = 2333;
constexpr static uint64_t tile = 1;
constexpr static uint64_t sigma = 8388608;
constexpr static double skewness = 0.0;

static inline uint64_t cache_tag_mask(uint64_t linesize, intptr_t addr) {
    return ((uint64_t)addr & (linesize - 1));
}

void do_sth(void *i)
{
    stop_watch<chrono::microseconds>(5);
}

using namespace std;
int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);
    cache_init();
    cache_t cache = cache_create_ronly(cache_size, cache_line_size, (char *)rbuf);

    vector<size_t> access_pattern = gen_access_pattern_seq(num_access_times, array_size);

    auto start = chrono::steady_clock::now();

    cache_token_t token;

    for (int i = 0; i < num_access_times; i += 1) {
        uint64_t addr = i * sizeof(uint64_t);

        // if need fetch
        if (!( i % per_line )) {
            token = cache_request(cache, addr);
        }

        cache_await(token);
        // do computation
        uint64_t *target = (uint64_t*) ((char *)cache_access_mut(token) + cache_tag_mask(cache_line_size, addr));
        *target = access_pattern[i];
    }

    for (int i = 0; i < num_access_times; i += 1) {
        uint64_t addr = i * sizeof(uint64_t);

        // if need fetch
        if (!( i % per_line )) {
            token = cache_request(cache, addr);
        }

        cache_await(token);
        // do computation
        uint64_t *target = (uint64_t*) ((char *)cache_access_mut(token) + cache_tag_mask(cache_line_size, addr));
        cout << i << " " << *target << endl;
    }

    auto end = chrono::steady_clock::now();
    cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
    return 0;
}
