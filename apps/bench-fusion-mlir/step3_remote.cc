#include <vector>
#include <cstdio>
#include <cstdlib>
#include "rring.h"
#include "internal.h"
#include <chrono>

using namespace std;

rring_init(rids, size_t, (2 << 20), 2047, 0, 8192);
rring_init(rvec, size_t, (2 << 20), 2047, 0, 8192 + (4ULL << 30));

const int n_ahead = 16;

template <typename D, typename I = size_t>
class MaxVisitor {
public:
  I index_ = 0;
  D max_ = 0;
  bool is_first = true;

  void pre() {}
  void post() {}
  void operator()(I idx, D dat) {
    // printf("%lu\n", dat);
    if (is_first || dat > max_) {
      max_ = dat;
      index_ = idx;
      is_first = false;
        // printf("update max %lu\n", max_);
    }
  }
  D get_result () const  { return (max_); }
  I get_index () const  { return (index_); }
};

template <typename D, typename I = size_t>
class MinVisitor {
public:
  I index_ = 0;
  D min_ = 0;
  bool is_first = true;

  void pre() {}
  void post() {}
  void operator()(I idx, D dat) {
    if (is_first || dat < min_) {
      min_ = dat;
      index_ = idx;
      is_first = false;
    }
  }
  D get_result () const  { return (min_); }
  I get_index () const  { return (index_); }
};

template <typename D, typename I = size_t>
class MeanVisitor {
public:
  D mean_ = 0;
  size_type cnt_ = 0;

  void pre() { mean_ = 0; cnt_ = 0; }
  void post() {}
  void operator()(I idx, D dat) {
    mean_ += dat;
    cnt_ ++;
  }
  size_type get_count () const  { return (cnt_); }
  D get_sum () const  { return (mean_); }
  D get_result () const  {
    return (mean_ / D(cnt_));
  }
  
};

template<typename V1, typename V2, typename V3>
void visit (V1 &visitor1, V2 &visitor2, V3 &visitor3)  {
  std::vector<size_t>& indices_ = *index_col;
  std::vector<uint64_t> &vec = *duration_col;

  const size_type idx_s = indices_.size();
  const size_type min_s = std::min<size_type>(vec.size(), idx_s);
  size_type       i = 0;

  visitor1.pre();
  visitor2.pre();
  visitor3.pre();

  // prologue
  for (int i = 0; i < n_ahead; ++ i) {
    rdma(_lbase_rvec + (i % _nblocks_rvec) * _bsize_rvec,
         _bsize_rvec, 
         _rbase_rvec + i * _bsize_rvec, 0, IBV_WR_RDMA_READ);
    rdma(_lbase_rids + (i % _nblocks_rids) * _bsize_rids,
        _bsize_rids, 
        _rbase_rids + i * _bsize_rids, i + 1, IBV_WR_RDMA_READ);
  }
  
  // outer loop
  rring_outer_loop_with(rvec, min_s);
  rring_outer_loop(rids, uint64_t, min_s) {
    // rring_prefetch_with(rids, rvec, n_ahead);
    // rring_prefetch(rids, n_ahead);
    if (_t_rids + n_ahead < _tlim_rids) {
      size_t _ip = (_t_rids + n_ahead);
      rdma(_lbase_rvec + (_ip % _nblocks_rvec) * _bsize_rvec,
          _bsize_rvec, 
          _rbase_rvec + _ip * _bsize_rvec, 0, IBV_WR_RDMA_READ);
      rdma(_lbase_rids + (_ip % _nblocks_rids) * _bsize_rids,
          _bsize_rids, 
          _rbase_rids + _ip * _bsize_rids, _ip + 1, IBV_WR_RDMA_READ);
    }

    rring_inner_preloop(rids, uint64_t);
    rring_inner_preloop(rvec, uint64_t);

    // rring_sync(rids);
    rring_poll_readonly(&_r_rids, _t_rids + 1);

    rring_inner_loop(rids, j) {
      visitor1 (_inner_rids[j], _inner_rvec[j]);
      visitor2 (_inner_rids[j], _inner_rvec[j]);
      visitor3 (_inner_rids[j], _inner_rvec[j]);
    }
    rring_outer_loop_with_post(rvec);
  }

  visitor3.post();
  visitor2.post();
  visitor1.post();
}

void calculate_trip_duration() {
    MaxVisitor<uint64_t> max_visitor;
    MinVisitor<uint64_t> min_visitor;
    MeanVisitor<uint64_t> mean_visitor;

    visit(max_visitor, min_visitor, mean_visitor);

    printf("Mean duration %lu seconds\n", mean_visitor.get_result());
    printf("Min duration %lu seconds\n", min_visitor.get_result());
    printf("Max duration %lu seconds\n", max_visitor.get_result());
    printf("\n");
}

int main () {
  ext_setup();
  _lbase_rids = (uint64_t)rbuf;
  _lbase_rvec = (uint64_t)rbuf + (4ULL << 30);

  printf("after setup\n");
  std::chrono::time_point<std::chrono::steady_clock> times[10];
  times[0] = std::chrono::steady_clock::now();
  calculate_trip_duration();
  times[1] = std::chrono::steady_clock::now();

  printf("Step 3: %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0]).count());
  return 0;
}
