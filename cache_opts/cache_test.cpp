#include <iostream>
#include "../common.h"
#include "../greeting.h"
#include "../cache.h"

constexpr static size_t max_size = 128;
constexpr static size_t cache_line_size = 4;

using namespace std;
int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);
    CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf);
    cache_access(cache, 0);
    struct req *missr = (struct req *) sbuf;
    cout << missr->addr << " " << missr->size << " " << missr->type << endl;
}
