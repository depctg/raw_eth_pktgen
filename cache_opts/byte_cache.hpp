#include <list>
#include <iostream>
#include <cmath>
#include <cstring>
#include <unordered_map>
#include <sparsehash/dense_hash_map>

#define MEMORY_ADDR_BITS 64 // S

// simple hash adapter for types without pointers
template<typename T> 
struct AddrHasher {
    size_t operator()(const T& t) const {
        return t;
    }    
};

namespace disagg_cache {
class cache {
    size_t max_size;
    size_t cache_line_size;
    uint64_t tag_mask;
    uint64_t addr_mask;
public:
    class Block {
    public:
        uint8_t present; // actually not needed, but left for optimization
        uint8_t dirty;
        uint64_t tag;
        char* line; // reference to data
        /* dll */
        Block *prev;
        Block *next;

        Block(uint8_t present, uint8_t dirty): present(present), dirty(dirty) {};

        Block(uint8_t present, uint8_t dirty, uint64_t tag, void* line): present(present), dirty(dirty), tag(tag), line((char *)line) {};
    };

    // max size of the cache table
    // excluding list etc
    int num_blocks;
    google::dense_hash_map<uint64_t, Block*, AddrHasher<uint64_t>> cache_table;
    Block *head = new Block(0, 0);
    Block *tail = new Block(0, 0);

    cache(size_t csize, size_t lsize): 
    max_size(csize), cache_line_size(lsize) {
        num_blocks = max_size / cache_line_size;
        uint8_t lbts = log2(cache_line_size);
        tag_mask = ((uint64_t) 1 << lbts) - 1;
        addr_mask = ~tag_mask;
        head->next = tail;
        tail->prev = head;
        cache_table.set_empty_key(-1);
        cache_table.set_deleted_key(-2);
    }

    // add to front of victim
    void add_block(Block *b) {
        Block *tmp = head->next;
        head->next = b;
        b->prev = head;
        b->next = tmp;
        tmp->next = tail;
    }

    void del_block(Block *b) {
        b->prev->next = b->next;
        b->next->prev = b->prev;
    }

    void insert(uint64_t addr, void* line) {
        uint64_t tag = addr & addr_mask;
        uint64_t offset = addr & tag_mask;
        if (cache_table.find(tag) != cache_table.end()) {
            Block *b = cache_table[tag];
            cache_table.erase(tag);
            del_block(b);
        }
        if (cache_table.size() == num_blocks) {
            cache_table.erase(tail->prev->tag);
            del_block(tail->prev);
        }
        // present and dirty
        add_block(new Block(1, 1, tag, line));
        cache_table[tag] = head->next;
    }

    bool get(uint64_t addr, char **buf) {
        uint64_t tag = addr & addr_mask;
        uint64_t offset = addr & tag_mask;
        if (cache_table.find(tag) != cache_table.end()) {
            Block *target = cache_table[tag];
            del_block(target);
            add_block(target);
            *buf = target->line + offset;
            return true;
        }
        // fetch from remote
        return false;
    }
};
}