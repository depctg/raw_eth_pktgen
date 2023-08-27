#ifndef _INTERNAL_H__
#define _INTERNAL_H__

#include <vector>

typedef size_t size_type;

extern void ext_setup();

extern std::vector<size_t> *index_col;
extern std::vector<uint64_t> *duration_col;

#endif
