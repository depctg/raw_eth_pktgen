#include <list>
#include <iostream>
#include <cmath>
#include <cstring>
#include "../common.h"
#include "../app.h"
#include "../packet.h"

using namespace std;

#define MEMORY_ADDR_BITS 64 // S
static size_t max_size = 256 << 20; // 2^M
static size_t cache_line_size = 4 << 10; // 2^L

namespace direct_mapped_cache {

class cache {
public:

typedef struct {
    int valid;
    int dirty;
    uint64_t tag;
    char* line; // payload with {cache line} bytes
} Block;

    Block* _blocks;
    int id_shifts; // = L
    int tag_shits; // = M - L 
    uint64_t id_mask;  //  = num blocks
    uint64_t tag_mask;  // = 2^(S-M)
    uint64_t miss_mask; // = ~1[L]

    // max number of cache blocks
    int num_blocks;

    struct req* reqs;
    char* recs;

    cache(size_t ms, size_t cs, void* s, void* r) {
        max_size = ms;
        cache_line_size = cs;
        num_blocks = max_size / cache_line_size;
        _blocks = new Block[num_blocks];
        for (int i = 0; i < num_blocks; ++i) { 
            _blocks[i].valid = 0;
            _blocks[i].dirty = 0;
            _blocks[i].tag = 0;
            _blocks[i].line = new char[cache_line_size];
        }
        // get shifts and masks
        int S = MEMORY_ADDR_BITS;
        int M = (int)log2(max_size);
        int L = (int)log2(cache_line_size);
        id_shifts = L;
        id_mask = ((uint64_t) 1 << (M-L)) - 1;
        tag_shits = M;
        tag_mask = ((uint64_t) 1 << (S-M)) - 1;
        miss_mask = ~(((uint64_t) 1 << L) - 1);
        init_bufs(s, r);
    }

    void init_bufs(void *s, void *r) {
        reqs = (struct req *) s;
        recs = (char *) r;
    }

    // /*
    // fetch content at offset
    // if failed, fetch from remote offset tag | index | 00000000
    void fetch(uint64_t offset, void* buf){
        // calculate index i
        const uint64_t a = (offset >> id_shifts);
        uint64_t index = (offset >> id_shifts) & id_mask;        
        // calcualte tag t
        uint64_t tag = (offset >> tag_shits) & tag_mask;
        // match block tag at index i
        Block &block = _blocks[index];
        if (block.valid && block.tag == tag) {
            memcpy(buf, block.line, sizeof(char));
            return;
        } else {
            // load from slow memory
            // return the base offset 
            reqs[0].index = offset & miss_mask;
            reqs[0].size = sizeof(char) * cache_line_size;
            send(reqs, sizeof(struct req));
            recv(recs, sizeof(char) * cache_line_size);
            block.tag = tag;
            // a lot of data copy
            memcpy(block.line, recs, sizeof(char) * cache_line_size);
            memcpy(buf, recs, sizeof(char) * cache_line_size);
            block.valid = 1;
            return;
        }
    }
    // */

    // insert content with offset
    void insert(uint64_t offset, void* line) {
        // calculate index i
        uint64_t index = (offset >> id_shifts) & id_mask;        
        // calcualte tag t
        uint64_t tag = (offset >> tag_shits) & tag_mask;
        Block &block = _blocks[index];
        if (block.dirty && block.valid) {
            // TODO: write-through
        }
        block.valid = 1;
        block.tag = tag;
        memcpy(block.line, line, cache_line_size * sizeof(char));
    }

    bool has_valid(uint64_t offset) {
        uint64_t index = (offset >> id_shifts) & id_mask;        
        // calcualte tag t
        uint64_t tag = (offset >> tag_shits) & tag_mask;
        Block &block = _blocks[index];
        return block.tag == tag && block.valid;
    }
};
}