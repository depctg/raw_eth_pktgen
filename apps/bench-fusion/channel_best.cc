#include <vector>
#include <cstdio>
#include "rvec.h"
#include <cstdlib>
#include "workload.hpp"

using namespace std;

void post_setup() {
  remotelize(2, indices);
  remotelize(3, v);
}

template<typename I, typename D, typename V1, typename V2, typename V3>
void visit (std::vector<I>& indices_, std::vector<D>& vec, V1 &visitor1, V2 &visitor2, V3 &visitor3)  {

  const size_type idx_s = indices_.size();
  const size_type min_s = std::min<size_type>(vec.size(), idx_s);
  size_type       i = 0;

  unsigned channel1 = channel_create(
    (uint64_t)&(indices_[0]), min_s, sizeof(I),
    sizeof(I), 1024, 1024, 0, 0, 0
  );
  unsigned channel2 = channel_create(
    (uint64_t)&(vec[0]), min_s, sizeof(D),
    sizeof(D), 1024, 1024, 0, 0, 0
  );

  visitor1.pre();
  visitor2.pre();
  visitor3.pre();
  for (; i < min_s; ++i) {
    I* idx = (I*) channel_access(channel1, i);
    D* di = (D*) channel_access(channel2, i);
    visitor1 (idx[0], di[0]);
    visitor2 (idx[0], di[0]);
    visitor3 (idx[0], di[0]);
  }
  visitor1.post();
  visitor2.post();
  visitor3.post();

  channel_destroy(channel2);
  channel_destroy(channel1);
}

int main () {

  init_client();
  cache_init();
  channel_init();

  do_work();
  return 0;
}