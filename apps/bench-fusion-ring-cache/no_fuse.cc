#include <vector>
#include <cstdio>
#include "rvec.h"
#include <cstdlib>
#include "workload.hpp"
#include "app.h"

using namespace std;

rring_init(rids, size_t, (2 << 20), 2047, 0, 8192);
rring_init(rvec, size_t, (2 << 20), 2047, 0, 8192 + (4ULL << 30));


template<typename I, typename D, typename V1, typename V2, typename V3>
void visit (std::vector<I>& indices_, std::vector<D>& vec, V1 &visitor1, V2 &visitor2, V3 &visitor3)  {

  const size_type idx_s = indices_.size();
  const size_type min_s = std::min<size_type>(vec.size(), idx_s);
  size_type       i = 0;

  virt_addr_t ser = { .ser = (uint64_t)(&indices_[0])};
  uint64_t indice_offset = RPC_RET_LIMIT + ser.addr;
  ser.ser = (uint64_t)(&vec[0]);
  uint64_t vec_offset = RPC_RET_LIMIT + ser.addr;
  {
    visitor1.pre();
    rring_outer_loop_with(rvec, min_s);
    rring_outer_loop(rids, uint64_t, min_s) {
      rring_prefetch_with(rids, rvec, 16);
      rring_prefetch(rids, 16);

      rring_inner_preloop(rids, uint64_t);
      rring_inner_preloop(rvec, uint64_t);

      rring_sync(rids);

      rring_inner_loop(rids, j) {
        visitor1 (_inner_rids[j], _inner_rvec[j]);
      }
      rring_outer_loop_with_post(rvec);
    }
    visitor1.post();
  }
  
  {
    visitor2.pre();
    rring_outer_loop_with(rvec, min_s);
    rring_outer_loop(rids, uint64_t, min_s) {
      rring_prefetch_with(rids, rvec, 16);
      rring_prefetch(rids, 16);

      rring_inner_preloop(rids, uint64_t);
      rring_inner_preloop(rvec, uint64_t);

      rring_sync(rids);

      rring_inner_loop(rids, j) {
        visitor2 (_inner_rids[j], _inner_rvec[j]);
      }
      rring_outer_loop_with_post(rvec);
    }
    visitor2.post();
  }
  {
    visitor3.pre();
    rring_outer_loop_with(rvec, min_s);
    rring_outer_loop(rids, uint64_t, min_s) {
      rring_prefetch_with(rids, rvec, 16);
      rring_prefetch(rids, 16);

      rring_inner_preloop(rids, uint64_t);
      rring_inner_preloop(rvec, uint64_t);

      rring_sync(rids);

      rring_inner_loop(rids, j) {
        visitor3 (_inner_rids[j], _inner_rvec[j]);
      }
      rring_outer_loop_with_post(rvec);
    }
    visitor3.post();
  }
}

int main () {

  init_client();

  _lbase_rids = (uint64_t)rbuf;
  _lbase_rvec = (uint64_t)rbuf + (4ULL << 30);

  setup();

  printf("bigin\n");

  rvector<size_t> * idxv = (rvector<size_t> *) &indices;
  size_t s = indices.size();
  size_t c = indices.capacity();

  rring_outer_loop(rids, size_t, c) {
    rring_inner_preloop(rids, size_t);
    rring_sync_writeonly(rids);
    rring_inner_loop(rids, j) {
      size_t nth = _t_rids * _bn_rids + j;
      _inner_rids[j] = idxv->head[nth];
    }
    rring_inner_wb(rids);
  }
  rring_cleanup_writeonly(rids);

  indices.clear();
  indices.shrink_to_fit();
  idxv->head = (size_t *) (4096ULL);
  idxv->end =  idxv->head + s;
  idxv->tail = idxv->head + c;

  printf("indices remote write complete\n");

  rvector<uint64_t> * vecv = (rvector<size_t> *) &v;
  s = v.size();
  c = v.capacity();

  rring_outer_loop(rvec, size_t, c) {
    rring_inner_preloop(rvec, size_t);
    rring_sync_writeonly(rvec);
    rring_inner_loop(rvec, j) {
      size_t nth = _t_rvec * _bn_rvec + j;
      _inner_rvec[j] = vecv->head[nth];
    }
    rring_inner_wb(rvec);
  }
  rring_cleanup_writeonly(rvec);

  v.clear();
  v.shrink_to_fit();
  vecv->head = (size_t *) (4096ULL + (4ULL << 30));
  vecv->end =  vecv->head + s;
  vecv->tail = vecv->head + c;
  
  printf("after setup\n");
  MaxVisitor<index_type, dat_type> maxVst;
  MinVisitor<index_type, dat_type> minVst;
  MeanVisitor<index_type, dat_type> meanVst;

  uint64_t start = microtime();
  visit(indices, v, maxVst, minVst, meanVst);
  uint64_t end = microtime();

  printf("Time = %lu us\n", end-start);
  printf("Max vst = %lu %lu\n", maxVst.index_, maxVst.max_);
  printf("Min vst = %lu %lu\n", minVst.index_, minVst.min_);
  printf("Mean vst = %lu %lu\n", meanVst.get_count(), meanVst.get_result());
  return 0;
}
