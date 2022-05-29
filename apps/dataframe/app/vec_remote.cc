#include <iostream>
#include "common.h"
#include "greeting.h"
#include "remoteKVS.hpp"
#include <assert.h>
#include <string>
#include <getopt.h>

using namespace std;

static struct option long_options[] = {
    {"addr", required_argument, 0, 0},
    {"cache_size", required_argument, 0, 0},
    {"cache_line_size", required_argument, 0, 0},
    {0, 0, 0, 0}
};

int main(int argc, char * argv[]) 
{
    char * addr = 0;
    uint64_t prefetch_size = 0;
    uint64_t cache_line_size = 8192;
    uint64_t cache_size = (480 << 20);
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
                break;
            case 4:
                break;
            default:
                return -1;
        }
    }
    if (!addr) return -1;

    init(TRANS_TYPE_RC_SERVER, addr);
    KVS *kvs = new KVS(sbuf, rbuf, cache_size, cache_line_size);
    kvs->serve();
}
