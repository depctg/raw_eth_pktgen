#include <iostream>
#include <sparsehash/dense_hash_map> // memory intense for efficiency
#include <cmath>
#include <string>
#include "greeting.h"


using namespace std;

// a queue that maintains compact sbuf usage
class FreeSlots
{
public:
  uint32_t front, end, frees, capacity;
  uint64_t* slots;

  // populate free slots
  FreeSlots(uint64_t max_size, uint32_t cache_line_size) :
    front(0),
    end(max_size / cache_line_size - 1),
    frees(max_size / cache_line_size),
    capacity(max_size / cache_line_size)
  {
    slots = new uint64_t[capacity];
    for (uint32_t i = 0; i < capacity; ++i)
    {
      slots[i] = i * cache_line_size;
    }
  }

  bool noFrees() { return frees == 0; }
  bool allFrees() { return frees == capacity; }
  // add a free slot back to queue
  void reclaim(uint64_t offset)
  {
    if (allFrees())
    {
      cout << "all ready all frees, wtf?" << endl;
      return;
    }
    else
    {
      end = (end + 1) % capacity;
      slots[end] = offset;
      frees += 1;
    }
  }

  // request a slot
  uint64_t claim()
  {
    if (noFrees()) return -1;
    uint64_t offset = slots[front];
    front = (front + 1) % capacity;
    frees -= 1;
    return offset;
  }
};

class KVS
{
// simple hash adapter for types without pointers
template<typename T> 
struct AddrHasher {
    size_t operator()(const T& t) const {
        return t;
    }    
};
public:
  char* cache_line_pool;
  // map [tag, offset]
  google::dense_hash_map<uint64_t, uint64_t, AddrHasher<uint64_t>> map;
  FreeSlots *slotManager;
  uint64_t max_size;
  uint32_t cache_line_size;
  uint64_t tag_mask;
  uint64_t addr_mask;
  uint8_t tag_shifts;

  KVS(void* send_buf, uint64_t max_size, uint32_t cache_line_size): 
  cache_line_pool((char*)send_buf), 
  max_size(max_size),
  cache_line_size(cache_line_size)
  {
    slotManager = new FreeSlots(max_size, cache_line_size);
    // lets hope we will never use these keys
    map.set_empty_key(-1);
    map.set_deleted_key(-2);
    tag_shifts = log2(cache_line_size * sizeof(char));
    tag_mask = ((uint64_t) 1 << tag_shifts) - 1;
    addr_mask = ~(tag_mask);
  }

  // uint64_t manual_insert(void *line)
  // {
  //   uint64_t offset = slotManager->claim();
  //   memcpy(cache_line_pool + offset, line, cache_line_size);
  //   cout << "manual insert " << line << "to offset " << offset << endl;
  //   return offset;
  // }

  void print_line(void *line)
  {
    uint64_t *u64l = (uint64_t *) line;
    for (uint8_t i = 0; i < cache_line_size * sizeof(char) / sizeof(uint64_t); ++i)
    {
      cout << u64l[i] << " ";
    }
    cout << endl;
  }

  void handle_req_fetch(struct req* r)
  {
    // no-copy
    // cout << "Fetch request addr, type, size: " << r->addr << " " << r->type << " " << r->size << endl;
    // print_line(cache_line_pool + map[r->addr]);
    uint64_t tag = (r->addr & addr_mask) >> tag_shifts;
    send(cache_line_pool + map[tag], cache_line_size);
  }

  void handle_req_update(struct req* r)
  {
    // print_line(r+1);
    uint64_t tag = (r->addr & addr_mask) >> tag_shifts;
    uint64_t line_offset = (r->addr & tag_mask);

    // insert type
    auto iter = map.find(tag);
    if (iter != map.end())
    {
      // update existing
      memcpy(cache_line_pool + iter->second + line_offset, /* payload after req*/ r + 1, r->size);
    }
    else
    {
      // insert new 
      // 1. request slot 2. memcpy from rbuf to sbuf
      // TODO: SGE to avoid copy
      uint64_t offset = slotManager->claim();
      memset(cache_line_pool + offset, 0, cache_line_size);
      memcpy(cache_line_pool + offset + line_offset, r+1, r->size); 
      // add to map
      map[tag] = offset;
    }
  }

};
