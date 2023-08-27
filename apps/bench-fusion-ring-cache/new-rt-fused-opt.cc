#include <vector>
#include <cstdio>
#include <cstdlib>
#include "workload.hpp"
#include "cache.hpp"
#include "rvector.h"
#include "common.h"

using namespace std;

const size_t s1_nb = 256 * 1024;
const size_t s1_n_block = 2048;

// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
using ridx = DirectCache<0,0,0,s1_n_block,s1_nb * sizeof(size_t),0>;
using rvec = DirectCache<s1_n_block,(4ULL<<30),(4ULL<<30),s1_n_block,s1_nb * sizeof(uint64_t),1>;

using ridx_R = CacheReq<ridx>;
using rvec_R = CacheReq<rvec>;

void post_setup() {
  new_remotelize<size_t, ridx, ridx_R>(indices, true);
  new_remotelize<uint64_t, rvec, rvec_R>(v, true);
}

const int n_ahead = 16;
const size_t n_lines = ARY_SIZE / s1_nb;

template<typename I, typename D, typename V1, typename V2, typename V3>
void visit (std::vector<I>& indices_, std::vector<D>& vec, V1 &visitor1, V2 &visitor2, V3 &visitor3) {

  const size_type idx_s = indices_.size();
  const size_type min_s = std::min<size_type>(vec.size(), idx_s);
  size_type       i = 0;

  visitor1.pre();
  visitor2.pre();
  visitor3.pre();

  int idx_offs[n_ahead+1];
  uint64_t idx_tags[n_ahead+1];
  int vec_offs[n_ahead+1];
  uint64_t vec_tags[n_ahead+1];
  size_t *idx_base = &indices_[0];
  uint64_t *vec_base = &vec[0];

  // prologue
  for (int i = 0; i < n_ahead; ++ i) {
    idx_tags[i] = ridx::Op::tag((uint64_t)(idx_base + i * s1_nb));
    idx_offs[i] = ridx::select(idx_tags[i]);
    ridx_R::request(idx_offs[i], idx_tags[i]);

    vec_tags[i] = rvec::Op::tag((uint64_t)(vec_base + i * s1_nb));
    vec_offs[i] = rvec::select(vec_tags[i]);
    rvec_R::request(vec_offs[i], vec_tags[i]);
  }

  // outer loop
  for (size_t j = 0; j < n_lines; ++ j) {
    // prefetch
    if (j < n_lines - n_ahead) {
      int idxn = (j + n_ahead) % (n_ahead + 1);

      idx_tags[idxn] = ridx::Op::tag((uint64_t)(idx_base + (j+n_ahead) * s1_nb));
      idx_offs[idxn] = ridx::select(idx_tags[idxn]);
      ridx_R::request(idx_offs[idxn], idx_tags[idxn]);

      vec_tags[idxn] = rvec::Op::tag((uint64_t)(vec_base + (j+n_ahead) * s1_nb));
      vec_offs[idxn] = rvec::select(vec_tags[idxn]);
      rvec_R::request(vec_offs[idxn], vec_tags[idxn]);
    }

    // sync
    int idx = j % (n_ahead + 1);
    auto &token = rvec::Op::token(vec_offs[idx]);
    poll_qid(rvec::Value::qid, token.seq);

    // inner loop
    size_t *idx_line_start = ridx::Op::template paddr<size_t>(idx_offs[idx], (uint64_t)(idx_base + j * s1_nb));
    uint64_t *vec_line_start = rvec::Op::template paddr<uint64_t>(vec_offs[idx], (uint64_t)(vec_base + j * s1_nb));
    for (size_t i = 0; i < s1_nb; ++ i) {
      size_t ie = idx_line_start[i];
      uint64_t de = vec_line_start[i];
      visitor1 (ie, de);
      visitor2 (ie, de);
      visitor3 (ie, de);
    }
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