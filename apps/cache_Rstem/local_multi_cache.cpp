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

    uint64_t cache_sizes[2] = { (1 << 5), (1 << 6) };
    uint64_t line_sizes[2] = { (1 << 3), (1 << 4) };
    uint64_t per_line[2] = { 
        line_sizes[0] / sizeof(uint64_t), 
        line_sizes[1] / sizeof(uint64_t) };
    
    cache_t cache0 = cache_create_ronly(cache_sizes[0], line_sizes[0], (char *)rbuf);
    cache_t cache1 = cache_create_ronly(cache_sizes[1], line_sizes[1], (char *) rbuf + cache_sizes[0]);

    vector<size_t> access_pattern0 = gen_access_pattern_seq(num_access_times, array_size);
    vector<size_t> access_pattern1 = gen_access_pattern_seq(num_access_times, num_access_times / 2);

    auto start = chrono::steady_clock::now();

    cache_token_t cache0_tokens[2] = { cache_request(cache0, 0) };
    cache_token_t cache1_tokens[2] = { cache_request(cache1, 0) };
    int tnxt[2] = { 0 };
    for (int i = 0; i < num_access_times; i += 1) {
        uint64_t addr = i * sizeof(uint64_t);
        uint64_t naddr = i * sizeof(uint64_t) + sizeof(uint64_t);
        int tcur0 = tnxt[0];
        int tcur1 = tnxt[1];

        // if need prefetch
        if (!( (i + 1) % per_line[0] )) {
            tnxt[0] = (tcur0 + 1) & 1;
            cache0_tokens[tnxt[0]] = cache_request(cache0, naddr);
        }
        if (!( (i + 1) % per_line[1] )) {
            tnxt[1] = (tcur1 + 1) & 1;
            cache1_tokens[tnxt[1]] = cache_request(cache1, naddr);
        }

        {
            cache_await(cache0_tokens[tcur0]);
            // do computation
            uint64_t *target0 = (uint64_t *) ((char *) cache_access_mut(cache0_tokens[tcur0]) + cache_tag_mask(line_sizes[0], addr));
            *target0 = access_pattern0[i];
        }
        {
            cache_await(cache1_tokens[tcur1]);
            // do computation
            uint64_t *target1 = (uint64_t *) ((char *) cache_access_mut(cache1_tokens[tcur1]) + cache_tag_mask(line_sizes[1], addr));
            *target1 = access_pattern1[i];

        }
    }

    tnxt[0] = tnxt[1] = 0;
    cache0_tokens[0] = cache_request(cache0, 0);
    cache1_tokens[0] = cache_request(cache1, 0);
    for (int i = 0; i < num_access_times; ++ i) {
        uint64_t addr = i * sizeof(uint64_t);
        uint64_t naddr = i * sizeof(uint64_t) + sizeof(uint64_t);
        int tcur0 = tnxt[0];
        int tcur1 = tnxt[1];

        // if need prefetch
        if (!( (i + 1) % per_line[0] )) {
            tnxt[0] = (tcur0 + 1) & 1;
            cache0_tokens[tnxt[0]] = cache_request(cache0, naddr);
        }

        if (!( (i + 1) % per_line[1] )) {
            tnxt[1] = (tcur1 + 1) & 1;
            cache1_tokens[tnxt[1]] = cache_request(cache1, naddr);
        }

        {
            cache_await(cache0_tokens[tcur0]);
            uint64_t *target0 = (uint64_t *) ((char *) cache_access(cache0_tokens[tcur0]) + cache_tag_mask(line_sizes[0], addr));
            printf("Cache0: %d %lu\n", i, *target0);
        }
        {
            cache_await(cache1_tokens[tcur1]);
            uint64_t *target1 = (uint64_t *) ((char *) cache_access(cache1_tokens[tcur1]) + cache_tag_mask(line_sizes[1], addr));
            printf("Cache1: %d %lu\n", i, *target1);

        }
    }

    auto end = chrono::steady_clock::now();
    cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
    return 0;
}
