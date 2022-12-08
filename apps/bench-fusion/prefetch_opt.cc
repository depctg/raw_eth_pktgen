#include <vector>
#include <cstdio>
#include "rvec.h"
#include <cstdlib>
#include "workload.hpp"
#include "rring.h"

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
  // should assert this is a multiple of base element size

  
  rring_init(idx, I, 1024, 256, rbuf, 0);
  rring_init(di, I, 1024, 256, rbuf, 1024*1024);

  rring_outer_loop(idx, I, min_s) {
      rring_outer_loop_with(di, min_s);

      rring_prefetch(idx, 16);
      rring_prefetch(di, 16);

      rring_inner_preloop(idx, I);
      rring_inner_preloop(di, I);

      rring_inner_loop(idx, j) {
          visitor1 (_inner_idx[j], _inner_di[j]);
          visitor2 (_inner_idx[j], _inner_di[j]);
          visitor3 (_inner_idx[j], _inner_di[j]);
      }
  }
}

int main () {

  init_client();
  cache_init();
  channel_init();

  do_work();
  return 0;
}
