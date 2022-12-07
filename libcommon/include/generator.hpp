#include <random>
#include <vector>
#include <cmath>
#include <stdlib.h>             
#include <math.h>               
#include <cassert>

using namespace std;

void ref_gen_uniform(size_t n_access, vector<size_t> &result, int seed = 2333) {
  std::mt19937 gen(seed);
  std::uniform_int_distribution<> d(0, n_access - 1);
  for (size_t i = 0; i < n_access; ++ i) {
    result.push_back(d(gen));
  }
}

void ref_gen_seq(size_t n_access, vector<size_t> &result)
{
  size_t p = 0;
  while (p < n_access)
  {
    result.push_back(p);
    p ++;
  }
}
