#include "common.h"
#include "pattern_generator.hpp"
#include <vector>
#include "rring.h"
#include "rvector.h"

std::vector<size_t> *index_col;
std::vector<uint64_t> *duration_col;

#define ARY_SIZE (4ULL << 27)
static std::vector<size_t> _index;
static std::vector<uint64_t> _dur;

rring_init(p_rids, size_t, (2 << 20), 2048, 0, 8192);
rring_init(p_rvec, size_t, (2 << 20), 2048, 0, 8192 + (4ULL << 30));

void ext_setup() {
  init_client();

  _index.reserve(ARY_SIZE);
  _dur.reserve(ARY_SIZE);

  ref_gen_uniform(ARY_SIZE, _dur, 2333);
  ref_gen_seq(ARY_SIZE, _index);

  index_col = &_index;
  duration_col = &_dur;

  _lbase_p_rids = (uint64_t)rbuf;
  _lbase_p_rvec = (uint64_t)rbuf + (4ULL << 30);

  printf("bigin\n");
  rvector<size_t> * idxv = (rvector<size_t> *) &_index;
  size_t s = _index.size();
  size_t c = _index.capacity();

  rring_outer_loop(p_rids, size_t, s) {
    rring_inner_preloop(p_rids, size_t);
    rring_sync_writeonly(p_rids);
    rring_inner_loop(p_rids, j) {
      size_t nth = _t_p_rids * _bn_p_rids + j;
      _inner_p_rids[j] = idxv->head[nth];
    }
    rring_inner_wb(p_rids);
  }
  rring_cleanup_writeonly(p_rids);

  _index.clear();
  _index.shrink_to_fit();
  idxv->head = (size_t *) (4096ULL);
  idxv->end =  idxv->head + s;
  idxv->tail = idxv->head + c;
  printf("indices remote write complete\n");

  rvector<uint64_t> * vecv = (rvector<size_t> *) &_dur;
  s = _dur.size();
  c = _dur.capacity();

  rring_outer_loop(p_rvec, size_t, s) {
    rring_inner_preloop(p_rvec, size_t);
    rring_sync_writeonly(p_rvec);
    rring_inner_loop(p_rvec, j) {
      size_t nth = _t_p_rvec * _bn_p_rvec + j;
      _inner_p_rvec[j] = vecv->head[nth];
    }
    rring_inner_wb(p_rvec);
  }
  rring_cleanup_writeonly(p_rvec);

  _dur.clear();
  _dur.shrink_to_fit();
  vecv->head = (size_t *) (4096ULL + (4ULL << 30));
  vecv->end =  vecv->head + s;
  vecv->tail = vecv->head + c;
  printf("duration remote write complete\n");
}