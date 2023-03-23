#include "common.h"
#include "tensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <immintrin.h>
#include "rring.h"

#define block_size (2L << 20)

// rring_init(rCw, float, block_size, n_block, 0, 8192 + (1ULL << 30));

#define B 64
#define C 64
#define S 224

// [64, 64]
const static int64_t strides2[2] = {64, 1};
static inline float *pin2(float *buf, int64_t a, int64_t b) {
  return buf + a * strides2[0] + b;
}

// [64, 64, 50176]
const static int64_t strides3[3] = {3211264, 50176, 1};
static inline float *pin3(float *buf, int64_t a, int64_t b, int64_t c) {
  return buf + a * strides3[0] + b * strides3[1] + c;
}

DECL_TENSOR(float, 2);
DECL_TENSOR(float, 4);
DEF_TENSOR_UTILS(float)

static inline void fma8(float *cv, float *av, float *bp) {
  for (int i = 0; i < 8; ++ i) {
    cv[i] += av[i] + bp[0];
  }
}

void conv_opt(Tensor_float_4 *image, Tensor_float_4 *pred) {
  rring_init(rA, float, block_size, 39, (uint64_t) rbuf + (8192ULL), 8192);
  float *x = image->_aligned_ptr;
  // read weights
  int64_t wshape[] = {C, C};
  float *w = read_tensor_float("/home/wuklab/projects/pl-zijian/raw_eth_pktgen/apps/bench-conv/constant_1", wshape, 2);

  // initialize pred
  int64_t oshape[] = {B, C, S, S};
  int64_t num_ele = 1;
  for (int i = 0; i < 4; ++ i)
    num_ele *= oshape[i];
  float *y = (float *) aligned_alloc(4096, sizeof(float) * num_ele);
  memset(y, 0, sizeof(float) * num_ele);

  __m256 alloca[4];
  __m256 bp;
  __m256 av;
  __m256 mul;
  __m256i ones = _mm256_set1_epi32(1);
  // matmul

for (int64_t i = 0; i < C; i += 4) {
  rring_outer_loop(rA, float, num_ele) {
    rring_prefetch(rA, 4);

    rring_inner_preloop(rA, float);
    rring_sync(rA);

    rring_inner_loop(rA, Ri) {
      if ((Ri & 63) != 0)
        continue;
      size_t nth = _t_rA * _bn_rA + Ri;
      float *xi = _inner_rA + Ri;
      int64_t n = nth / strides3[0];
      int64_t k = (nth % strides3[0]) / strides3[1];
      if (k > 56)
        break;
      int64_t j = (nth % strides3[0]) % strides3[1];
      if (j > strides3[1] - 8)
        break;

  // for (int64_t n = 0; n < B; ++ n) {
  //     for (int64_t j = 0; j < S * S; j += 8) {
  //       for (int64_t k = 0; k < C; k += 8) {
          // prepare C block
          alloca[0] = _mm256_loadu_ps(pin3(y, n, i, j));
          alloca[1] = _mm256_loadu_ps(pin3(y, n, i + 1, j));
          alloca[2] = _mm256_loadu_ps(pin3(y, n, i + 2, j));
          alloca[3] = _mm256_loadu_ps(pin3(y, n, i + 3, j));
          // alloca[0] = _mm256_loadu_ps(yi + k * 8 + j);
          // alloca[1] = _mm256_loadu_ps(yi + (k+1) * 8 + j);
          // alloca[2] = _mm256_loadu_ps(yi + (k+2) * 8 + j);
          // alloca[3] = _mm256_loadu_ps(yi + (k+3) * 8 + j);

          // float alloca[4][8];
          // memcpy(alloca[0], pin3(y, n, i, j), sizeof(float) * 8);
          // memcpy(alloca[1], pin3(y, n, i + 1, j), sizeof(float) * 8);
          // memcpy(alloca[2], pin3(y, n, i + 2, j), sizeof(float) * 8);
          // memcpy(alloca[3], pin3(y, n, i + 3, j), sizeof(float) * 8);

          // C[4,8] = 4 x Av * Bp
          for (int64_t z = 0; z < 8; z += 4) {
            for (int64_t ii = 0; ii < 4; ++ ii) {
              for (int64_t zz = z; zz < z + 4; ++ zz) {
                int64_t s = k + zz;
                bp = _mm256_broadcast_ss(pin2(w, i + ii, s));
                // av = _mm256_loadu_ps(pin3(x, n, s, j));
                av = _mm256_loadu_ps(xi + s * 8 + j);
                mul = _mm256_mul_ps(bp, av);
                alloca[ii] = _mm256_add_ps(mul, alloca[ii]);
                // float *bp = pin2(w, i + ii, s);
                // float *av = pin3(x, n, s, j);
                // fma8(alloca[ii], av, bp);
              }
            }
          }
          _mm256_maskstore_ps(pin3(y, n, i, j), ones, alloca[0]);
          _mm256_maskstore_ps(pin3(y, n, i + 1, j), ones, alloca[0]);
          _mm256_maskstore_ps(pin3(y, n, i + 2, j), ones, alloca[0]);
          _mm256_maskstore_ps(pin3(y, n, i + 3, j), ones, alloca[0]);

          // _mm256_maskstore_ps(yi + k * 8 + j, ones, alloca[0]);
          // _mm256_maskstore_ps(yi + (k+1) * 8 + j, ones, alloca[0]);
          // _mm256_maskstore_ps(yi + (k+2) * 8 + j, ones, alloca[0]);
          // _mm256_maskstore_ps(yi + (k+3) * 8 + j, ones, alloca[0]);
          // memcpy(pin3(y, n, i, j), alloca[0], sizeof(float) * 8);
          // memcpy(pin3(y, n, i + 1, j), alloca[1], sizeof(float) * 8);
          // memcpy(pin3(y, n, i + 2, j), alloca[2], sizeof(float) * 8);
          // memcpy(pin3(y, n, i + 3, j), alloca[3], sizeof(float) * 8);
    //     }
    //   }
    // }
  }
    }
  }
  *pred = make_tensor_float_4(y, oshape);
}

int main () {
  init_client();
  rring_init(rA, float, block_size, 32, (uint64_t) rbuf + (8192ULL), 8192);
  rring_init(rC, float, block_size, 32, (uint64_t) rbuf + (8192ULL) + (1ULL << 30), 8192 + (1ULL << 30));
  // _lbase_rCw = (uint64_t) rbuf + (8192ULL) + (1ULL << 30);

  int64_t shape[] = {B, C, S, S};
  size_t num_ele = 1;
  for (int i = 0; i < 4; ++ i)
    num_ele *= shape[i];
  float *data = read_tensor_float("/home/wuklab/projects/pl-zijian/raw_eth_pktgen/apps/bench-conv/dummy_in.dat", shape, 4);

  rring_outer_loop(rA, float, num_ele) {
    rring_inner_preloop(rA, float);
    rring_sync_writeonly(rA);
    rring_inner_loop(rA, i) {
      size_t nth = _t_rA * _bn_rA + i;
      _inner_rA[i] = data[nth];
    }
    rring_inner_wb(rA);
  }
  rring_cleanup_writeonly(rA);

  Tensor_float_4 X = make_tensor_float_4(data, shape);
  
  Tensor_float_4 y;
  uint64_t start = microtime();
  conv_opt(&X, &y);
  uint64_t end = microtime();
  printf("Exec time %.6fs\n", (float)(end-start)/1e6);


  return 0;
}