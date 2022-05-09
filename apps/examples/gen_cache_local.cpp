#include <iostream>
#include <memory>
#include <cstdlib>
#include "common.h"
#include "greeting.h"
#include "cache.h"

typedef struct MPLD {
  uint64_t A;
  uint32_t B;
} MPLD;

// Assumption:

// Currently one cache table should be dedicated to only one
// data type

// When storing to cache, the addr is not totally arbitrary,
// make sure the addr is aligned to the size of target data

// For instance, size(MPLD) = 16, the cache line size should be
// 16 * K, the access/write addr should be 16 * N as well. 

constexpr static uint64_t kCacheSize = sizeof(MPLD) * 16;
constexpr static uint64_t kCacheLineSize = sizeof(MPLD) * 8;
constexpr static uint64_t sbuf_ofst = 2 << 20;
constexpr static uint64_t rbuf_ofst = 2 << 20;

using namespace std;
int main(int argc, char * argv[]) {
  if (argc != 2) {
      printf("Usage: %s <connection-key\n", argv[0]);
      exit(1);
  }
	init(TRANS_TYPE_RC, argv[1]);

  // Init cache table with total_size and cache_line_size
  // sbuf and rbuf can be offseted if you want to leave some space
  // for other usage
  CacheTable *cache = createCacheTable(kCacheSize, kCacheLineSize, (char *) sbuf + sbuf_ofst, (char *) rbuf + rbuf_ofst);

  // Init local payload
  MPLD *lp = (MPLD *) malloc(sizeof(MPLD) * 10);
  for (int i = 0; i < 10; ++i)
  {
    lp[i].A = i + 1;
    lp[i].B = i << 1;
  }
  cout << sizeof(MPLD) << endl;
  // write arbitrary length to arbitrary addr
  // We write to addr 16 * 3, the total size stored is 16 * 10
  // since the cache line is of size 16 * 8, we expect the virtual 
  // memory layout in the cache table is:
  // CL_0: | 0 ... 0 ... | MPLD 0, ..., MPLD 4 | 
  // CL_1: | MPLD 5, ..., MPLD 9 | 0 ... 0 ... | 
  cache_write_n(cache, sizeof(MPLD) * 3, lp, sizeof(MPLD) * 10);

  // Access MPLD 6
  // Addr = 16 * 3 + 16 * 6
  // Tag = 1, Line Offset = 16
  MPLD *p6 = (MPLD *) cache_access(cache, sizeof(MPLD) * 9);
  cout << p6->A << " " << p6->B << endl;

  // Write another payload to local, addr = 16 * 16, tag = 2
  // Since max size is 2 cache line, now all occupied
  // write at new position will cause one cache line to be evicted
  // LRU will evict the first cache line, tag = 0
  MPLD *rp = (MPLD *) malloc(sizeof(MPLD));
  rp->A = 11;
  rp->B = 20;
  cache_write_n(cache, kCacheLineSize * 2, rp, sizeof(MPLD));

  // Then access MPLD within address [0, cache line size] will 
  // cause a fetch from remote
  // will fetch the entire cache line (tag = 0) from remote
  // (tag = 1) will be evicted to make room
  // addr of p3 is 16 * 3 + 16 * 3
  MPLD *p3 = (MPLD *) cache_access(cache, sizeof(MPLD) * 6);
  cout << p3->A << " " << p3->B << endl;

  // Write to remote 
  // No cache line will be evicted, the eviction status will not be changed
  // will merge with local cache line if presented -> dirty status remains intact
  // Write drp to addr 16 * 16 + 16, tag = 2, line_ofst = 16
  // right after rp, in the same cache line (presented)
  MPLD *drp = (MPLD *) malloc(sizeof(MPLD));
  drp->A = 12;
  drp->B = 22;
  uint64_t addr = kCacheLineSize * 2 + sizeof(MPLD);
  remote_write_n(cache, addr, drp, sizeof(MPLD));
  MPLD *read_from_local = (MPLD *) cache_access(cache, addr);
  cout << read_from_local->A << " " << read_from_local->B << endl;

  // write to remote
  // Write to un-presented cache line, tag = 3
  // no eviction, mamerialized when do local access
  // In this case all presented cache lines are clean
  // No send operation performed when evicting
  MPLD *drp_new = (MPLD *) malloc(sizeof(MPLD));
  drp_new->A = 13;
  drp_new->B = 24;
  uint64_t addr_new = kCacheLineSize * 3;
  remote_write_n(cache, addr_new, drp_new, sizeof(MPLD));
  MPLD *read_from_remote = (MPLD *) cache_access(cache, addr_new);
  cout << read_from_remote->A << " " << read_from_remote->B << endl;
  
  // Prefetch will asynchronously fetch a cache line
  // Try write to remote directly and pre-fetch it
  MPLD *pflp = (MPLD *) malloc(kCacheLineSize);
  for (int i = 0; i < kCacheLineSize / sizeof(MPLD); ++i)
  {
    pflp[i].A = i + 1;
    pflp[i].B = i << 1;
  }
  uint64_t addr_pf = 5 * kCacheLineSize; // tag = 5
  remote_write_n(cache, addr_pf, pflp, kCacheLineSize);
  prefetch(cache, addr_pf);

  MPLD *pflp2 = (MPLD *) cache_access(cache, addr_pf + 2 * sizeof(MPLD));
  cout << pflp2->A << " " << pflp2->B << endl;
}
