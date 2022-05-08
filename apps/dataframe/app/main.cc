#include <DataFrame/DataFrame.h>
#include <DataFrame/DataFrameStatsVisitors.h>
#include <DataFrame/RandGen.h>

#include <iostream>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <getopt.h>

#include "common.h"
#include "cache.h"
#include "vec.hpp"

static uint64_t max_size = (480 << 20);
static uint32_t cache_line_size = 8192;

using namespace hmdf;

typedef StdDataFrame<time_t> MyDataFrame;

static struct option long_options[] = {
    {"addr", required_argument, 0, 0},
    {"prefetch_size", required_argument, 0, 0},
    {0, 0, 0, 0}
};

int main(int argc, char * argv[])
{
    char * addr = 0;
    uint64_t prefetch_size = 0;

    int opt= 0, long_index =0;
    while ((opt = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1) {
        switch (long_index) {
            case 0:
                addr = optarg;
                break;
            case 1:
                prefetch_size = atoll(optarg);
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                return -1;
        }
    }

    if (!addr) return -1;

    auto kCacheSize = max_size;
    auto kCacheLineSize = cache_line_size;
    init(TRANS_TYPE_RC, addr);

    MyDataFrame::set_thread_level(1);

    CacheTable *cache = createCacheTable(kCacheSize, kCacheLineSize, sbuf, rbuf);
    RCacheVector_init_cache_table(cache);

    MyDataFrame             df;

    const size_t            index_sz =
        df.load_index(
            MyDataFrame::gen_datetime_index("01/01/1970",
                                            "01/02/1970",
                                            time_frequency::secondly,
                                            1));
    RandGenParams<double>   p;

    p.mean = 1.0;  // Default
    p.std = 0.005;

    df.load_column("normal", gen_normal_dist<double>(index_sz, p));
    df.load_column("log_normal", gen_lognormal_dist<double>(index_sz));
    p.lambda = 1.5;
    df.load_column("exponential", gen_exponential_dist<double>(index_sz, p));

    std::cout << "All memory allocations are done. Calculating means ..."
              << std::endl;

    MeanVisitor<double, time_t> n_mv;
    MeanVisitor<double, time_t> ln_mv;
    MeanVisitor<double, time_t> e_mv;

    // doesn't work due to clone()
    // auto    fut1 = df.visit_async<double>("normal", n_mv);
    // auto    fut2 = df.visit_async<double>("log_normal", ln_mv);
    // auto    fut3 = df.visit_async<double>("exponential", e_mv);
    // fut1.get();
    // fut2.get();
    // fut3.get();

    // auto    fut1 = df.visit<double>("normal", n_mv);
    // auto    fut2 = df.visit<double>("log_normal", ln_mv);
    // auto    fut3 = df.visit<double>("exponential", e_mv);

    std::vector<std::chrono::time_point<std::chrono::steady_clock>> times();

    if (prefetch) {
        times.emplace(std::chrono::steady_clock::now());
        auto    fut1 = df.visit_prefetch<double>("normal", n_mv, prefetch_size);
        times.emplace(std::chrono::steady_clock::now());
        auto    fut2 = df.visit_prefetch<double>("log_normal", ln_mv, prefetch_size);
        times.emplace(std::chrono::steady_clock::now());
        auto    fut3 = df.visit_prefetch<double>("exponential", e_mv, prefetch_size);
        times.emplace(std::chrono::steady_clock::now());
    } else {
        times.emplace(std::chrono::steady_clock::now());
        auto    fut1 = df.visit<double>("normal", n_mv);
        times.emplace(std::chrono::steady_clock::now());
        auto    fut2 = df.visit<double>("log_normal", ln_mv);
        times.emplace(std::chrono::steady_clock::now());
        auto    fut3 = df.visit<double>("exponential", e_mv);
        times.emplace(std::chrono::steady_clock::now());
    }
    // std::cout << "Normal mean " << n_mv.get_result() << std::endl;
    // std::cout << "Log Normal mean " << ln_mv.get_result() << std::endl;
    // std::cout << "Exponential mean " << e_mv.get_result() << std::endl;


    for (uint32_t i = 1; i < std::size(times); i++) {
        std::cout << "Step " << i << ": "
                  << std::chrono::duration_cast<std::chrono::microseconds>(times[i] - times[i - 1])
                         .count()
                  << " us" << std::endl;
    }
    std::cout << "Total: "
              << std::chrono::duration_cast<std::chrono::microseconds>(times.back() - times.front()).count()
              << " us" << std::endl;


    return (0);
}