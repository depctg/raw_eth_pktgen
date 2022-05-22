#include <iostream>
#include <sparsehash/dense_hash_map> // memory intense for efficiency
#include <cmath>
#include <string>
#include "common.h"
#include "greeting.h"


using namespace std;

template <class T>
class FreeSlots {
public:
  uint32_t front, end, frees, capacity;
  T* slots;

  FreeSlots(T *flist, uint32_t l) {
    front = 0;
    end = l - 1;
    frees = l;
    capacity = l;

    slots = flist;
  }
  bool noFrees() { return frees == 0; };
  bool allFrees() { return frees == capacity; };

  void reclaim(T give_back) {
    if (allFrees()) {
      cout << "All ready all frees" << endl;
      return;
    } else {
      end = (end + 1) % capacity;
      slots[end] = give_back;
      frees += 1;
    }
  };

  T claim() {
    if (noFrees()) {
      cout << "All give away" << endl;
      return -1;
    }
    T ret = slots[front];
    front = (front + 1) % capacity;
    frees -= 1;
    return ret;
  };
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
  char* client_reqs;
  // map [tag, offset]
  google::dense_hash_map<uint64_t, uint64_t, AddrHasher<uint64_t>> map;
  FreeSlots<uint64_t> *sbuf_manager;
  // FreeSlots<uint64_t> *rbuf_manager;
  uint64_t max_size;
  uint64_t cache_line_size;
  uint64_t tag_mask;
  uint64_t addr_mask;
  uint8_t tag_shifts;

  KVS(void* send_buf, void* recv_buf, uint64_t max_size, uint64_t cache_line_size): 
  cache_line_pool((char*)send_buf), 
  client_reqs((char *)recv_buf),
  max_size(max_size),
  cache_line_size(cache_line_size) {
    uint64_t l = max_size / cache_line_size;
    uint64_t *free_clp = new uint64_t[l];
    for (uint64_t i = 0; i < l; ++ i)
      free_clp[i] = cache_line_size * i;
    sbuf_manager = new FreeSlots<uint64_t>(free_clp, l);

    // to restrict recv inflights
    // uint64_t max_rr_l = CQ_NUM_DESC / 2;
    // uint64_t *free_rr = new uint64_t[max_rr_l];
    // for (uint64_t i = 0; i < max_rr_l; ++ i)
    //   free_rr[i] = (cache_line_size + sizeof(struct req)) * i;
    // rbuf_manager = new FreeSlots<uint64_t>(free_rr, max_rr_l);

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

  void print_line(void *line, size_t length) {
    uint64_t *u64l = (uint64_t *) line;
    for (uint8_t i = 0; i < length * sizeof(char) / sizeof(uint64_t); ++i) {
      cout << u64l[i] << " ";
    }
    cout << endl;
  }

  void handle_req_fetch(struct req* r) {
    // no-copy
    // cout << "Fetch request addr, type, size: " << r->addr << " " << r->type << " " << r->size << endl;
    uint64_t tag = (r->addr & addr_mask) >> tag_shifts;
    // print_line(cache_line_pool + map[tag], cache_line_size);
    send_async(cache_line_pool + map[tag], cache_line_size);
  }

  void handle_req_update(struct req* r) {
    // print_line(r+1);
    uint64_t tag = r->addr >> tag_shifts;
    uint64_t line_offset = (r->addr & tag_mask);

    // insert type
    auto iter = map.find(tag);
    if (iter != map.end()) {
      // update existing
      memcpy(cache_line_pool + iter->second + line_offset, /* payload after req*/ r + 1, r->size);
    } else {
      // insert new 
      // 1. request slot 2. memcpy from rbuf to sbuf
      // TODO: SGE to avoid copy
      uint64_t offset = sbuf_manager->claim();
      // memset(cache_line_pool + offset, 0, cache_line_size);
      memcpy(cache_line_pool + offset + line_offset, r+1, r->size); 
      // add to map
      map[tag] = offset;
    }
  }

  void serve() {
    struct ibv_wc wcs[MAX_POLL];
    size_t req_size = sizeof(struct req) + cache_line_size;
    uint64_t recv_polls = 0, recv_posts = 0;
    int max_recv_inflights = MAX_POLL / 2;
    for (int i = 0; i < max_recv_inflights; ++ i) {
      recv_async(client_reqs + i * req_size, req_size);
    }
    recv_posts += max_recv_inflights;
    while (1) {
      int n = poll_cq(cq, MAX_POLL, wcs);
      for (int i = 0; i < n; ++ i) {
        if (wcs[i].status == IBV_WC_SUCCESS && wcs[i].opcode == IBV_WC_RECV) {
          uint64_t idx = (recv_polls ++) % MAX_POLL;
          struct req *r = (struct req *) (client_reqs + idx * req_size);
          // const char *type = r->type == 1 ? "Fetch" : "Update";
          // cout << "Req " << r->addr << ", tag " << (r->addr >> kvs->tag_shifts)  << ", size " << r->size << ", type " << type << endl;
          if (r->type)
            handle_req_fetch(r);
          else
            handle_req_update(r);
        }
      }
      while (recv_posts < recv_polls + max_recv_inflights) {
        int idx = (recv_posts ++) % MAX_POLL;
        recv_async(client_reqs + idx * req_size, req_size);
      }
    }
  }
};
