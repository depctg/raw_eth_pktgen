#include <vector>
#include <cstdio>
#include <cstdlib>
#include "workload.hpp"
#include "cache.hpp"
#include "rvector.h"
#include "common.h"

using namespace std;

const size_t s1_nb = 512 * 1024;
const size_t s1_n_block = 32;

// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
using ridx = DirectCache<0,0,0,s1_n_block,s1_nb * sizeof(size_t),0>;
using rvec = DirectCache<s1_n_block,(4ULL<<30),(4ULL<<30),s1_n_block,s1_nb * sizeof(uint64_t),1>;

using ridx_R = CacheReq<ridx>;
using rvec_R = CacheReq<rvec>;

void post_setup() {
  new_remotelize<size_t, ridx, ridx_R>(indices, true);
  new_remotelize<uint64_t, rvec, rvec_R>(v, true);
}

template<typename I, typename D, typename V1, typename V2, typename V3>
void visit (std::vector<I>& indices_, std::vector<D>& vec, V1 &visitor1, V2 &visitor2, V3 &visitor3) {

  const size_type idx_s = indices_.size();
  const size_type min_s = std::min<size_type>(vec.size(), idx_s);
  size_type       i = 0;

  visitor1.pre();
  visitor2.pre();
  visitor3.pre();

  for (i = 0; i < min_s; ++i) {
    size_t ie = *ridx_R::get<size_t>(&indices_[i]);
    uint64_t de = *rvec_R::get<uint64_t>(&vec[i]);
    visitor1 (ie, de);
    visitor2 (ie, de);
    visitor3 (ie, de);
  }

  visitor1.post();
  visitor2.post();
  visitor3.post();
}

int main () {
  init_client();
  do_work();
  return 0;
}