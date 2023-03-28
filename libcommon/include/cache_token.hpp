#ifndef _CACHE_TOKEN_HPP_
#define _CACHE_TOKEN_HPP_

#include <stdint.h>

#ifndef NUM_TOKENS
    #define NUM_TOKENS (1024 * 1024)
#endif

struct Token {
    uint64_t tag;
    uint8_t flags;
    uint8_t pad0;
    uint16_t seq;
    uint32_t meta2;
    
    // methods
    inline bool valid() { return flags & 0x1; } 
    inline bool dirty() { return flags & 0x2; } 
    inline bool sync() { return flags & 0x4; } 

    // since v = 0x01
    inline void set(uint8_t flag) { flags = flag; } 
    inline void add(uint8_t flag) { flags &= flag; } 
    inline void clear() { flags = 0; } 

    static const uint8_t Valid = 0x1;
    static const uint8_t Dirty = 0x2;
    static const uint8_t Sync = 0x4;
};

extern Token tokens[NUM_TOKENS];

#endif
