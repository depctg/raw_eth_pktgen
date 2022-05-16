#include <DataFrame/DataFrame.h>
#include <DataFrame/DataFrameStatsVisitors.h>
#include <DataFrame/RandGen.h>

#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <memory>
#include <getopt.h>

#include "common.h"
#include "cache.h"
#include "vec.hpp"

using namespace hmdf;

typedef StdDataFrame<uint64_t> MyDataFrame;

static struct option long_options[] = {
    {"addr", required_argument, 0, 0},
    {"cache_size", required_argument, 0, 0},
    {"cache_line_size", required_argument, 0, 0},
    {"prefetch_n", required_argument, 0, 0},
    {"index_size", required_argument, 0, 0},
    {0, 0, 0, 0}
};

int main(int argc, char * argv[])
{
    char * addr = 0;
    uint64_t prefetch_size = 0;
    uint64_t cache_line_size = 8192;
    uint32_t cache_size = (480 << 20);
    uint64_t index_size = 100;

    int opt= 0, long_index =0;
    while ((opt = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1) {
        switch (long_index) {
            case 0:
                addr = optarg;
                break;
            case 1:
                cache_size = atoll(optarg);
                break;
            case 2:
                cache_line_size = atoll(optarg);
                break;
            case 3:
                prefetch_size = atoll(optarg);
                break;
            case 4:
                index_size = atoll(optarg);
                break;
            default:
                return -1;
        }
    }

    if (!addr) return -1;

    init(TRANS_TYPE_RC, addr);

    std::cout << "Connected. Prepare data" << std::endl;

    CacheTable *cache = createCacheTable(cache_size, cache_line_size, sbuf, rbuf);
    RCacheVector_init_cache_table(cache);

    MyDataFrame::set_thread_level(1);
    MyDataFrame df;

    const size_t index_sz = df.load_index(MyDataFrame::gen_sequence_index(0, index_size));

    const char * col_data = "data", * col_data2 = "data2";
    RandGenParams<double> p;
    p.mean = 1.0;  // Default
    p.std = 0.005;
    df.load_column(col_data, gen_normal_dist<double>(index_sz, p));
    p.mean = 2.0;
    p.std = 0.003;
    df.load_column(col_data2, gen_normal_dist<double>(index_sz, p));
    // df.load_column("normal", gen_normal_dist<double>(index_sz, p));
    // df.load_column("log_normal", gen_lognormal_dist<double>(index_sz));
    // p.lambda = 1.5;
    // df.load_column("exponential", gen_exponential_dist<double>(index_sz, p));

    std::cout << "Memory allocation bytes: " << index_sz * 2 * sizeof(double) << std::endl;

    // single col
    MeanVisitor<double, uint64_t> v_mean; // uses 3 local var
    NLargestVisitor<5, double> v_5nl; // uses std::array
    StatsVisitor<double> v_stats, v_warmup; // uses 6 local var

    // double col
    SLRegressionVisitor<double> v_slr; // uses 2 stats visitor
    DotProdVisitor<double> v_dotp;

    {
        int warmup = 3;
        while (warmup--) df.visit<double>(col_data, v_warmup);
    }

    std::vector<std::pair<std::chrono::time_point<std::chrono::steady_clock>, std::string>> times;

    if (prefetch_size) {
        times.emplace_back(std::chrono::steady_clock::now(), "p mean");
        df.visit_prefetch<double>(col_data, v_mean, prefetch_size);
        times.emplace_back(std::chrono::steady_clock::now(), "p mean");

        times.emplace_back(std::chrono::steady_clock::now(), "p 5nl");
        df.visit_prefetch<double>(col_data, v_5nl, prefetch_size);
        times.emplace_back(std::chrono::steady_clock::now(), "p 5nl");

        times.emplace_back(std::chrono::steady_clock::now(), "p stats");
        df.visit_prefetch<double>(col_data, v_stats, prefetch_size);
        times.emplace_back(std::chrono::steady_clock::now(), "p stats");

        times.emplace_back(std::chrono::steady_clock::now(), "p dotp");
        df.visit_prefetch<double, double>(col_data, col_data2, v_dotp, prefetch_size);
        times.emplace_back(std::chrono::steady_clock::now(), "p dotp");

        times.emplace_back(std::chrono::steady_clock::now(), "p slr");
        df.visit_prefetch<double, double>(col_data, col_data2, v_slr, prefetch_size);
        times.emplace_back(std::chrono::steady_clock::now(), "p slr");
    } else {
        times.emplace_back(std::chrono::steady_clock::now(), "mean");
        df.visit<double>(col_data, v_mean);
        times.emplace_back(std::chrono::steady_clock::now(), "mean");

        times.emplace_back(std::chrono::steady_clock::now(), "5nl");
        df.visit<double>(col_data, v_5nl);
        times.emplace_back(std::chrono::steady_clock::now(), "5nl");

        times.emplace_back(std::chrono::steady_clock::now(), "stats");
        df.visit<double>(col_data, v_stats);
        times.emplace_back(std::chrono::steady_clock::now(), "stats");

        times.emplace_back(std::chrono::steady_clock::now(), "dotp");
        df.visit<double, double>(col_data, col_data2, v_dotp);
        times.emplace_back(std::chrono::steady_clock::now(), "dotp");

        times.emplace_back(std::chrono::steady_clock::now(), "slr");
        df.visit<double, double>(col_data, col_data2, v_slr);
        times.emplace_back(std::chrono::steady_clock::now(), "slr");
    }

    std::cerr << prefetch_size;
    for (uint32_t i = 1; i < std::size(times); i+=2) {
        auto p = std::chrono::duration_cast<std::chrono::microseconds>(times[i].first - times[i - 1].first).count();
        std::cout << times[i].second << ": "
                  << p
                  << " us" << std::endl;
        std::cerr << "\t" << p;
    }
    std::cerr << std::endl;

    std::cout << "Total: "
              << std::chrono::duration_cast<std::chrono::microseconds>(times.back().first - times.front().first).count()
              << " us" << std::endl;


    return (0);
}