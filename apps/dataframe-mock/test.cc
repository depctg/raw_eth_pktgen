#include <vector>
#include <cstdio>
#include "rvec.h"
#include <cstdlib>
#include "offload.h"

using namespace std;

// int sum(vector<int> &v) {
//   int s = 0;
//   size_t ss = v.size();
//   for (size_t i = 0; i < ss; ++ i) {
//     s += v[i];
//   }
//   return s;
// }

typedef size_t size_type;

typedef size_t index_type;
typedef uint64_t dat_type;

class Visitor {
 public:
  uint64_t sum = 0;
  void pre() {}
  void post() {}
  void operator()(int a, int b) {sum += a; sum += b;}  
};


template <typename I, typename D>
class MaxVisitor {
public:
  I index_ = 0;
  D max_ = 0;
  bool is_first = true;

  void pre() {}
  void post() {}
  void operator()(I idx, D dat) {
    if (is_first || dat > max_) {
      max_ = dat;
      index_ = idx;
      is_first = false;
    }
  }
};

template <typename I, typename D>
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
};

template<typename I, typename D, typename V1, typename V2>
static inline void visit (std::vector<I>& indices_, std::vector<D>& vec, V1 &visitor1, V2 &visitor2)  {

  const size_type idx_s = indices_.size();
  const size_type min_s = std::min<size_type>(vec.size(), idx_s);
  size_type       i = 0;

  unsigned channel1 = channel_create(
    (uint64_t)&(indices_[0]), min_s, sizeof(I),
    sizeof(I), 32, 32, 0, 0, 0
  );
  unsigned channel2 = channel_create(
    (uint64_t)&(vec[0]), min_s, sizeof(D),
    sizeof(D), 32, 32, 0, 0, 0
  );
  visitor1.pre();
  for (; i < min_s; ++i) {
    I* idx = (I*) channel_access(channel1, i);
    D* di = (D*) channel_access(channel2, i);
    visitor1 (idx[0], di[0]);
  }
  visitor1.post();
  channel_destroy(channel2);
  channel_destroy(channel1);

  channel1 = channel_create(
    (uint64_t)&(indices_[0]), min_s, sizeof(I),
    sizeof(I), 32, 32, 0, 0, 0
  );
  channel2 = channel_create(
    (uint64_t)&(vec[0]), min_s, sizeof(D),
    sizeof(D), 32, 32, 0, 0, 0
  );
  visitor2.pre();
  for (i = 0; i < min_s; ++i) {
    I* idx = (I*) channel_access(channel1, i);
    D* di = (D*) channel_access(channel2, i);
    visitor2 (idx[0], di[0]);
  }
  visitor2.post();
  channel_destroy(channel2);
  channel_destroy(channel1);
}

template <typename T>
struct __rv {
  T *p1, *p2, *p3;
};

int main () {

  init_client();
  cache_init();
  channel_init();

  vector<dat_type> v {0, 1, 2, 3, 4, 5};
  vector<index_type> indices {0, 1, 2, 3, 4, 5};

  remotelize(v);
  remotelize(indices);

  __rv<index_type> *rv = (__rv<index_type> *) &indices;
  memcpy(offload_arg_buf, rv, sizeof(*rv));
  index_type sum = *(index_type *) call_offloaded_service(0, sizeof(*rv), sizeof(index_type));
  printf("Offload sum = %ld\n", sum);

  MaxVisitor<index_type, dat_type> maxVst;
  MinVisitor<index_type, dat_type> minVst;

  visit(indices, v, maxVst, minVst);

  printf("Max vst = %lu %lu\n", maxVst.index_, maxVst.max_);
  printf("Min vst = %lu %lu\n", minVst.index_, minVst.min_);

  return 0;
}