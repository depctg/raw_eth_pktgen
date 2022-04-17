
// req layout:
// | ------------> header <--------------   |
// | addr (64b) | size (32b) | type (32b)   | payload
// CL: cache line payloud

#ifndef _GREETING_
#define _GREETING_

struct req {
    uint64_t addr;
    uint32_t size;
    uint32_t type;
};

// followed by payload

#endif