#include <random>
#include <vector>
#include <cmath>
#include <stdlib.h>             
#include <stdio.h>
#include <math.h>               
#include <cassert>

using namespace std;

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


double rand_val(int seed)
{
  const long  a =      16807;  // Multiplier
  const long  m = 2147483647;  // Modulus
  const long  q =     127773;  // m div a
  const long  r =       2836;  // m mod a
  static long x;               // Random int value
  long        x_div_q;         // x divided by q
  long        x_mod_q;         // x modulo q
  long        x_new;           // New x value

  // Set the seed if argument is non-zero and then return zero
  if (seed > 0)
  {
    x = seed;
    return(0.0);
  }

  // RNG using integer arithmetic
  x_div_q = x / q;
  x_mod_q = x % q;
  x_new = (a * x_mod_q) - (r * x_div_q);
  if (x_new > 0)
    x = x_new;
  else
    x = x_new + m;

  // Return a random value between 0.0 and 1.0
  return((double) x / m);
}

vector<size_t> gen_access_pattern_seq(size_t n_access, size_t array_size)
{
  vector<size_t> pattern;
  size_t p = 0;
  while (p < n_access)
  {
    pattern.push_back(p % array_size);
    p ++;
  }
  return pattern;
}
