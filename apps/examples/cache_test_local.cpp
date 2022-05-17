#include <iostream>
#include <memory>
#include <cstdlib>
#include "common.h"
#include "greeting.h"
#include "cache.h"

constexpr static uint64_t kCacheSize = 8000;
constexpr static uint64_t kCacheLineSize = 1000;
constexpr static uint64_t sbuf_ofst = 2 << 20;
constexpr static uint64_t rbuf_ofst = 2 << 20;

using namespace std;
int main(int argc, char * argv[]) {
  if (argc != 2) {
      printf("Usage: %s <connection-key\n", argv[0]);
      exit(1);
  }
	init(TRANS_TYPE_RC, argv[1]);

  CacheTable *cache = createCacheTable(kCacheSize, kCacheLineSize, (char *) sbuf + sbuf_ofst, (char *) rbuf + rbuf_ofst);

  uint64_t *dat = new uint64_t[2000];
  for (int i = 0; i < 2000; ++ i)
    dat[i] = i;

  for (int i = 0; i < 2000; ++ i)
    cache_write(cache, 16000, dat);
  uint64_t *tgt = (uint64_t*) cache_access(cache, 16000);
  cout << *tgt << endl;
}
