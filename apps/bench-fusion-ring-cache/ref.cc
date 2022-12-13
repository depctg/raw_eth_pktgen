#include <vector>
#include <cstdio>
#include <cstdlib>
#include "workload.hpp"

using namespace std;

void post_setup() { return; }

template<typename I, typename D, typename V1, typename V2, typename V3>
void visit (std::vector<I>& indices_, std::vector<D>& vec, V1 &visitor1, V2 &visitor2, V3 &visitor3) {

  const size_type idx_s = indices_.size();
  const size_type min_s = std::min<size_type>(vec.size(), idx_s);
  size_type       i = 0;

  visitor1.pre();
  for (; i < min_s; ++i) {
    visitor1 (indices_[i], vec[i]);
  }
  visitor1.post();

  visitor2.pre();
  for (i = 0; i < min_s; ++i) {
    visitor2 (indices_[i], vec[i]);
  }
  visitor2.post();

  visitor3.pre();
  for (i = 0; i < min_s; ++i) {
    visitor3 (indices_[i], vec[i]);
  }
  visitor3.post();

}

int main () {

  do_work();
  return 0;
}