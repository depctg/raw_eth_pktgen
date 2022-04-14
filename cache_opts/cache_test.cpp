#include <iostream>
#include "../common.h"
#include "../greeting.h"
#include "../cache.h"

constexpr static uint64_t max_size = 16 << 20;
constexpr static uint32_t cache_line_size = 64;

using namespace std;
int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);
    CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf);
    uint64_t *v  = (uint64_t*) cache_access(cache, 64);
    cout << "Get " << *v << endl;
    return 1;
}
