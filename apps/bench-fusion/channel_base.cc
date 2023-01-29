#include <vector>
#include <cstdio>
#include "rvec.h"
#include <cstdlib>
#include "workload.hpp"
#include <cstdint>
#include "rring.h"
#include "app.h"

using namespace std;

void post_setup() {
  remotelize(2, v);
  remotelize(3, indices);
}

template<typename I, typename D, typename V1, typename V2, typename V3>
void visit (std::vector<I>& indices_, std::vector<D>& vec, V1 &visitor1, V2 &visitor2, V3 &visitor3)  {

  const size_type idx_s = indices_.size();
  const size_type min_s = std::min<size_type>(vec.size(), idx_s);
  size_type       i = 0;

  virt_addr_t ser = { .ser = (uint64_t)(&indices_[0])};
  uint64_t indice_offset = RPC_RET_LIMIT + ser.addr;
  ser.ser = (uint64_t)(&vec[0]);
  uint64_t vec_offset = RPC_RET_LIMIT + ser.addr;

  visitor1.pre();
  rring_init(rids0, uint64_t, access_block_size, 16, (size_t)ring_start_base, indice_offset);
  rring_init(rvec0, uint64_t, access_block_size, 16, (size_t)ring_start_base + (32<<20), vec_offset);

  rring_outer_loop_with(rvec0, min_s);
  rring_outer_loop(rids0, uint64_t, min_s) {
    rring_prefetch_with(rids0, 2);
    rring_prefetch(rvec0, 2);

    rring_inner_preloop(rids0, uint64_t);
    rring_inner_preloop(rvec0, uint64_t);

    rring_sync(rvec0);

    rring_inner_loop(rids0, j) {
      visitor1 (_inner_rids0[j], _inner_rvec0[j]);
    }
    rring_outer_loop_with_post(rvec0);
  }
  visitor1.post();

  visitor2.pre();
  rring_init(rids1, uint64_t, access_block_size, 16, (size_t)ring_start_base + (64<<20),indice_offset);
  rring_init(rvec1, uint64_t, access_block_size, 16, (size_t)ring_start_base + (96<<20), vec_offset);

  rring_outer_loop_with(rvec1, min_s);
  rring_outer_loop(rids1, uint64_t, min_s) {
    rring_prefetch_with(rids1, 2);
    rring_prefetch(rvec1, 2);

    rring_inner_preloop(rids1, uint64_t);
    rring_inner_preloop(rvec1, uint64_t);

    rring_sync(rvec1);

    rring_inner_loop(rids1, j) {
      visitor2 (_inner_rids1[j], _inner_rvec1[j]);
    }
    rring_outer_loop_with_post(rvec1);
  }
  visitor2.post();

  visitor3.pre();
  rring_init(rids2, uint64_t, access_block_size, 16, (size_t)ring_start_base + (128<<20), indice_offset);
  rring_init(rvec2, uint64_t, access_block_size, 16, (size_t)ring_start_base + (160<<20), vec_offset);

  rring_outer_loop_with(rvec2, min_s);
  rring_outer_loop(rids2, uint64_t, min_s) {
    rring_prefetch_with(rids2, 2);
    rring_prefetch(rvec2, 2);

    rring_inner_preloop(rids2, uint64_t);
    rring_inner_preloop(rvec2, uint64_t);

    rring_sync(rvec2);

    rring_inner_loop(rids2, j) {
      visitor3 (_inner_rids2[j], _inner_rvec2[j]);
    }
    rring_outer_loop_with_post(rvec2);
  }
  visitor3.post();

}

int main () {

  init_client();
  cache_init();
  channel_init();

  do_work();
  return 0;
}