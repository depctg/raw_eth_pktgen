#include <iostream>
#include <memory>
#include <cstdlib>
#include "common.h"
#include "greeting.h"
#include "cache.h"

constexpr static uint64_t CacheSize = 256 << 20;
constexpr static uint64_t CacheLineSize = 4 << 10;
constexpr static uint64_t sbuf_ofst = 2 << 20;
constexpr static uint64_t rbuf_ofst = 2 << 20;

using namespace std;
int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);
  cache_t local_cache = cache_create(kCacheSize, kCacheLineSize, )
}
