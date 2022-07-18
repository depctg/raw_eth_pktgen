#include <iostream>
#include <chrono>
#include <assert.h>

#include "common.h"
#include "clock.hpp"
#include "cache.h"
#include "patterns.hpp"

constexpr static uint64_t array_size = 1 << 10;
constexpr static uint64_t num_access_times = 1 << 5;
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

    uint64_t cache_sizes[2] = { (1 << 5), (1 << 6) };
    uint64_t line_sizes[2] = { (1 << 3), (1 << 4) };
    uint64_t per_line[2] = { 
        line_sizes[0] / sizeof(uint64_t), 
        line_sizes[1] / sizeof(uint64_t) };
    
    cache_t cache0 = cache_create(cache_sizes[0], line_sizes[0], (char *)rbuf);
    cache_t cache1 = cache_create(cache_sizes[1], line_sizes[1], (char *)rbuf + (cache_sizes[0]/line_sizes[0]) * (sizeof(line_header) + line_sizes[0]));

    vector<size_t> access_pattern0 = gen_access_pattern_seq(num_access_times, array_size);
    vector<size_t> access_pattern1 = gen_access_pattern_seq(num_access_times, num_access_times / 2);

    auto start = chrono::steady_clock::now();

    cache_token_t cache0_token;
    cache_token_t cache1_token;
    for (int i = 0; i < num_access_times; i += 1) {
        uint64_t addr = i * sizeof(uint64_t);

        // if need fetch
        if (!( i % per_line[0] )) {
            cache0_token = cache_request(cache0, addr);
        }
        if (!( i % per_line[1] )) {
            cache1_token = cache_request(cache1, addr);
        }

        {
            cache_await(cache0_token);
            // do computation
            uint64_t *target0 = (uint64_t *) ((char *) cache_access_mut(cache0_token) + cache_tag_mask(line_sizes[0], addr));
            *target0 = access_pattern0[i];
        }
        {
            cache_await(cache1_token);
            // do computation
            uint64_t *target1 = (uint64_t *) ((char *) cache_access_mut(cache1_token) + cache_tag_mask(line_sizes[1], addr));
            *target1 = access_pattern1[i];

        }
    }

    for (int i = 0; i < num_access_times; ++ i) {
        uint64_t addr = i * sizeof(uint64_t);

        // if need fetch
        if (!( i % per_line[0] )) {
            cache0_token = cache_request(cache0, addr);
        }
        if (!( i % per_line[1] )) {
            cache1_token = cache_request(cache1, addr);
        }

        {
            cache_await(cache0_token);
            // do computation
            uint64_t *target0 = (uint64_t *) ((char *) cache_access_mut(cache0_token) + cache_tag_mask(line_sizes[0], addr));
            printf("Cache0: %d %lu\n", i, *target0);
        }
        {
            cache_await(cache1_token);
            // do computation
            uint64_t *target1 = (uint64_t *) ((char *) cache_access_mut(cache1_token) + cache_tag_mask(line_sizes[1], addr));
            printf("Cache1: %d %lu\n", i, *target1);

        }
    }

    auto end = chrono::steady_clock::now();
    cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
    return 0;
}
