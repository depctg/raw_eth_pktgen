#include <iostream>
#include "./byte_cache.hpp"

using namespace std;
int main(int argc, char **argv) {
    const uint64_t cs = 4<<10;
    const uint32_t cls = 16;
    disagg_cache::cache c(cs, cls);
    char hw[cls] = "hello";
    c.insert(0, hw);

    char *g = (char *) malloc(sizeof(char));
    c.get(0, &g);
    cout << g[0] << endl;
    return 0;
}