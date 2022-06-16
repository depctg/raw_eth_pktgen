#include <iostream>
#include <chrono>
#include <assert.h>

#include "common.h"
#include "cache.h"
#include "patterns.hpp"
#include "clock.hpp"

constexpr static uint64_t cache_size = 1 << 6;
constexpr static uint64_t cache_line_size = 1 << 5;
constexpr static uint64_t per_line = cache_line_size / (sizeof(uint64_t));

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

    cache_t cache1 = cache_create_ronly(cache_size, cache_line_size, (char *)rbuf);
    cache_t cache2 = cache_create_ronly(cache_size, cache_line_size, (char *)rbuf + cache_size);

    // vector<size_t> access_pattern = gen_access_pattern_normal(num_access_times, array_size / 2, 1, sigma, 2333);
    // vector<size_t> access_pattern = gen_access_pattern_uniform(num_access_times, array_size, tile, seed);
    vector<size_t> access_pattern1 = gen_access_pattern_seq(num_access_times, array_size);
    vector<size_t> access_pattern2 = gen_access_pattern_seq(num_access_times, num_access_times / 2);
    // vector<size_t> access_pattern = gen_access_pattern_zipf(num_access_times, array_size, skewness, 1, 10);

    auto start = chrono::steady_clock::now();

    const int num_tokens = 4;
    cache_token_t tokens[4] = { cache_request(cache1, 0) };

    for (int i = 0; i < num_access_times / per_line; i += 1) {
        uint64_t addr = i * per_line * sizeof(uint64_t);
        dprintf("i %d line %d, addr %lx", i, cache_line_size, addr);

        int tcur = i % num_tokens;
        int tnxt = (i + 1) % num_tokens;
        if (i < num_access_times / per_line - 1) {
            tokens[tnxt] = cache_request(cache1, addr + cache_line_size);
        }

        cache_await(tokens[tcur]);
        // do computation
        uint64_t *line = (uint64_t*) cache_access_mut(tokens[tcur]);
        for (int j = 0; j < per_line; ++ j) {
            line[j] = access_pattern1[i * per_line + j];
        }
    }

    cache_token_t tokens2[4] = { cache_request(cache2, 0) };

    tokens[0] = cache_request(cache1, 0);
    for (int i = 0; i < num_access_times / per_line; i += 1) {
        uint64_t addr = i * per_line * sizeof(uint64_t);
        dprintf("i %d line %d, addr %lx", i, cache_line_size, addr);

        int tcur = i % num_tokens;
        int tnxt = (i + 1) % num_tokens;
        if (i < num_access_times / per_line - 1) {
            tokens[tnxt] = cache_request(cache1, addr + cache_line_size);
        }

        cache_await(tokens[tcur]);
        cout << "line1: " << endl;
        uint64_t *line = (uint64_t*) cache_access_mut(tokens[tcur]);
        for (int j = 0; j < per_line; ++ j) {
            cout << line[j] << " ";
        }
        cout << endl;

    }

    auto end = chrono::steady_clock::now();
    cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
    return 0;
}
