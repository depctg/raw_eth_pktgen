#include <iostream>
#include <sparsehash/dense_hash_map> // memory intense for efficiency
#include <cmath>
#include <string>
#include "../greeting.h"


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
  google::dense_hash_map<uint64_t, uint64_t, AddrHasher<uint64_t>> map;
  FreeSlots *slotManager;
  uint64_t max_size;
  uint32_t cache_line_size;

  KVS(void* send_buf, uint64_t max_size, uint32_t cache_line_size): 
  cache_line_pool((char*)send_buf), 
  max_size(max_size),
  cache_line_size(cache_line_size),
  slotManager(new FreeSlots(max_size, cache_line_size))
  {
    // lets hope we will never use these keys
    map.set_empty_key(-1);
    map.set_deleted_key(-2);
  }

  // uint64_t manual_insert(void *line)
  // {
  //   uint64_t offset = slotManager->claim();
  //   memcpy(cache_line_pool + offset, line, cache_line_size);
  //   cout << "manual insert " << line << "to offset " << offset << endl;
  //   return offset;
  // }

  void handle_req_fetch(struct req* r)
  {
    // fetch type, send send req
    cout << "First 5 data in this line: " << endl;
    for (uint8_t i = 0; i < 5; ++i)
    {
      cout << *((uint64_t*) (cache_line_pool + map[r->addr]) + i) << endl;
    }
    // no-copy
    send_async(cache_line_pool + map[r->addr], cache_line_size);
  }

  void handle_req_update(struct req* r)
  {
    // insert type
    auto iter = map.find(r->addr);
    if (iter != map.end())
    {
      // update existing
      memcpy(cache_line_pool + iter->second, /* payload after req*/ r + 1, r->size);
    }
    else
    {
      // insert new 1. request slot 2. memcpy from rbuf to sbuf
      uint64_t offset = slotManager->claim();
      memcpy(cache_line_pool + offset, r+1, r->size); 
      // add to map
      map[r->addr] = offset;
    }
  }

};
