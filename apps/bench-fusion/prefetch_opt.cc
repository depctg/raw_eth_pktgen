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

  
}

int main () {

  init_client();
  cache_init();
  channel_init();

  size_t sum = 0, sum2 = 0;

  // ring buf size = 1024 * 32
  // local cache: [rbase, rbase + 1024 *32]
  // remote: [sbuf + 0, sbuf + 0 + 1024 * 32]

#if 0
  rring_init(writer, int, 1024, 32, rbuf, 0);
  rring_init(writer2, int, 1024, 32, rbuf + 1024*32, 1024 * 1024 * sizeof(int));

  // 1024 * 1024 -> number of elements
  rring_outer_loop(writer, int, 1024UL * 1024) {
  rring_outer_loop_with(writer2, int);
      // keep this!
      rring_inner_preloop(writer, int);
      rring_inner_preloop(writer2, int);

      rring_inner_loop(writer, j) {
        size_t acc = _t_writer * _bn_writer + j;
        // nth = _t_<name> * _bn_<name> + j;
        // cur_elm = _inner_<name>[j]
        _inner_writer[j] = acc;
        _inner_writer2[j] = acc;
        sum += acc;
      }

      // write back current line
      rring_inner_wb(writer);
  }
#endif
  // name, Type, block size, #blocks, local base, remote base
  rring_init(writer, int, 1024, 32, rbuf, 0);

  // 1024 * 1024 -> number of elements
  rring_outer_loop(writer, int, 1024UL * 1024) {
      // keep this!
      rring_inner_preloop(writer, int);
      rring_sync_writeonly(writer);

      rring_inner_loop(writer, j) {
        size_t acc = _t_writer * _bn_writer + j;
        // nth = _t_<name> * _bn_<name> + j;
        // cur_elm = _inner_<name>[j]
        _inner_writer[j] = acc;
        sum += acc;
      }

      // write back current line
      rring_inner_wb(writer);
  }

  rring_init(idx, int, 1024, 256, rbuf, 0);
  rring_outer_loop(idx, int, 1024UL * 1024) {
      rring_prefetch(idx, 16);
      rring_inner_preloop(idx, int);
      rring_sync(idx);
      rring_inner_loop(idx, j) {
        sum2 += _inner_idx[j];
      }
  }

  printf("FINISH %ul %ul\n", sum, sum2);

  return 0;
}
