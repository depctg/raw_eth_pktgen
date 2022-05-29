#include <iostream>
#include <chrono>
#include <assert.h>

#include "common.h"
#include "cache.h"
#include "patterns.hpp"
#include "clock.hpp"

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
    cache_init();

    cache_t cache = cache_create(1024 * 4, cache_line_size,
            (char *)sbuf + 1024 * 1024 * 4, (char *)rbuf);

    // vector<size_t> access_pattern = gen_access_pattern_normal(num_access_times, array_size / 2, 1, sigma, 2333);
    // vector<size_t> access_pattern = gen_access_pattern_uniform(num_access_times, array_size, tile, seed);
    vector<size_t> access_pattern = gen_access_pattern_seq(num_access_times, array_size);
    // vector<size_t> access_pattern = gen_access_pattern_zipf(num_access_times, array_size, skewness, 1, 10);

    auto start = chrono::steady_clock::now();

    const int num_tokens = 4;
    cache_token_t tokens[4] = { cache_request(cache, 0) };

    for (int i = 0; i < num_access_times; i += 1) {
        uint64_t addr = (i * cache_line_size) % (1024 * 64);

        dprintf("i %d line %d, addr %lx", i, cache_line_size, addr);

        int tcur = i % num_tokens;
        int tnxt = (i + 1) % num_tokens;
        tokens[tnxt] = cache_request(cache, addr + cache_line_size);

        cache_await(tokens[tcur]);
        // do computation
        cache_access_mut(tokens[tcur]);
    }

    auto end = chrono::steady_clock::now();

    return 0;
}
