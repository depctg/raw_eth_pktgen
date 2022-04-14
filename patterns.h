#include <random>
#include <vector>
#include <cmath>

using std::size_t;
using std::vector;

vector<size_t> gen_access_pattern_uniform(size_t n_access, size_t array_size,
                                          size_t tile_size, int seed = 0) {
  std::mt19937 gen(seed);
  std::uniform_int_distribution<> d(0, array_size - 1);
  vector<size_t> pattern;
  size_t t_i = 0, p_i = 0;
  while (p_i < n_access) {
    if (t_i == 0) {
      pattern.push_back(d(gen));
      p_i += 1;
      t_i = 1;
    } else if (t_i < tile_size) {
      size_t next = pattern.back() + 1;
      if (next >= array_size) {
        t_i = tile_size;
      } else {
        pattern.push_back(next);
        p_i += 1;
        t_i += 1;
      }
    } else if (t_i == tile_size) {
      t_i = 0;
    }
  }
  return pattern;
}

vector<size_t> gen_access_pattern_normal(size_t n_access, size_t array_size,
                                         size_t tile_size, double mean, double stddev = 1.0, int seed = 0) {
  std::mt19937 gen(seed);
  std::normal_distribution<> d{mean, stddev};
  vector<size_t> pattern;
  size_t t_i = 0, p_i = 0;
  while (p_i < n_access) {
    if (t_i == 0) {
      pattern.push_back(static_cast<size_t>(std::abs(d(gen))) % array_size);
      p_i += 1;
      t_i = 1;
    } else if (t_i < tile_size) {
      size_t next = pattern.back() + 1;
      if (next >= array_size) {
        t_i = tile_size;
      } else {
        pattern.push_back(next);
        p_i += 1;
        t_i += 1;
      }
    } else if (t_i == tile_size) {
      t_i = 0;
    }
  }
  return pattern;
}