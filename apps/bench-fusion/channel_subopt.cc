#include <vector>
#include <cstdio>
#include "rvec.h"
#include <cstdlib>
#include "workload.hpp"
#include "rring.h"
#include "app.h"

using namespace std;

void post_setup() {
  remotelize(2, v);
  remotelize(3, indices);
}

#define ring_start_base ((char*)rbuf + (8UL << 20))

template<typename I, typename D, typename V1, typename V2, typename V3>
void visit (std::vector<I>& indices_, std::vector<D>& vec, V1 &visitor1, V2 &visitor2, V3 &visitor3)  {

  const size_type idx_s = indices_.size();
  const size_type min_s = std::min<size_type>(vec.size(), idx_s);
  size_type       i = 0;

  // unsigned channel1 = channel_create(
  //   (uint64_t)&(indices_[0]), min_s, sizeof(I),
  //   sizeof(I), 128, 128, 0, 0, 0
  // );
  // unsigned channel2 = channel_create(
  //   (uint64_t)&(vec[0]), min_s, sizeof(D),
  //   sizeof(D), 128, 128, 0, 0, 0
  // );
  virt_addr_t ser = { .ser = (uint64_t)(&indices_[0])};
  uint64_t indice_offset = RPC_RET_LIMIT + ser.addr;
  ser.ser = (uint64_t)(&vec[0]);
  uint64_t vec_offset = RPC_RET_LIMIT + ser.addr;

  printf("%lx %lx\n", indice_offset, vec_offset);

  rring_init(rids, uint64_t, (2 << 20), 32, (size_t)ring_start_base,            indice_offset);
  rring_init(rvec, uint64_t, (2 << 20), 32, (size_t)ring_start_base + (64<<20), vec_offset);

  rring_outer_loop_with(rvec, min_s);
  rring_outer_loop(rids, uint64_t, min_s) {

    rring_prefetch(rids, 16);
    rring_prefetch(rvec, 16);

    rring_inner_preloop(rids, uint64_t);
    rring_inner_preloop(rvec, uint64_t);

    rring_sync(rvec);

    rring_inner_loop(rids, j) {
      visitor1 (_inner_rids[j], _inner_rvec[j]);
      visitor2 (_inner_rids[j], _inner_rvec[j]);
      visitor3 (_inner_rids[j], _inner_rvec[j]);
    }

    rring_outer_loop_with_post(rvec);
  }


  // visitor1.pre();
  // visitor2.pre();
  // visitor3.pre();
  // for (; i < min_s; ++i) {
  //   I* idx = (I*) channel_access(channel1, i);
  //   D* di = (D*) channel_access(channel2, i);
  //   visitor1 (idx[0], di[0]);
  //   visitor2 (idx[0], di[0]);
  //   visitor3 (idx[0], di[0]);
  // }
  // visitor1.post();
  // visitor2.post();
  // visitor3.post();

  // channel_destroy(channel2);
  // channel_destroy(channel1);
}

int main () {

  init_client();
  cache_init();
  channel_init();

  // uint64_t sum = 0;
  // rring_init(rids, uint64_t, 1024, 32, (size_t)rbuf,            0);
  // rring_init(rvec, uint64_t, 1024, 32, (size_t)rbuf + (32<<10), 0 + 1024 * 1024 * sizeof(uint64_t));

  // rring_outer_loop_with(rvec, 1024*1024);
  // rring_outer_loop(rids, uint64_t, 1024*1024) {

  //   rring_prefetch(rids, 2);
  //   rring_prefetch(rvec, 2);

  //   rring_inner_preloop(rids, uint64_t);
  //   rring_inner_preloop(rvec, uint64_t);

  //   rring_sync(rvec);

  //   rring_inner_loop(rids, j) {
  //     sum += (_inner_rids[j] + _inner_rvec[j]);
  //   }

  //   rring_outer_loop_with_post(rvec);
  // }

  do_work();
  return 0;
}
