#include "common.h"
#include "pattern_generator.hpp"
#include <vector>

std::vector<size_t> *index_col;
std::vector<uint64_t> *duration_col;

#define ARY_SIZE (4ULL << 27)
static std::vector<size_t> _index;
static std::vector<uint64_t> _dur;

void ext_setup() {
  _index.reserve(ARY_SIZE);
  _dur.reserve(ARY_SIZE);

  ref_gen_uniform(ARY_SIZE, _dur, 2333);
  ref_gen_seq(ARY_SIZE, _index);

  index_col = &_index;
  duration_col = &_dur;
}