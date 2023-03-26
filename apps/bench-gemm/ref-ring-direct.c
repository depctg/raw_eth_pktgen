#include "common.h"
#include "tensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <immintrin.h>
#include "cache.h"
#include "rring.h"

#define M 64512
#define K 512
#define N 512

DECL_TENSOR(float, 2);
DEF_TENSOR_UTILS(float)

const static int64_t strides2[2] = {N, 1};
static inline float *pin2(float *buf, int64_t a, int64_t b) {
  return buf + a * strides2[0] + b;
}

void _mlir_ciface_main_graph(Tensor_float_2 *C, Tensor_float_2 *A, Tensor_float_2 *B) {

  uint64_t _rC = (uint64_t) _disagg_alloc(2, (256ULL << 20));
  uint64_t _lC = (uint64_t) rbuf + (8192ULL) + (256ULL << 20) + (1ULL << 20);
  rring_init(wrC, float, 2048 * 4, 8064, _lC, _rC);

  int64_t oshape[] = {M, N};
  int64_t num_ele = 1;
  for (int i = 0; i < 2; ++ i)
    num_ele *= oshape[i];

  rring_outer_loop(wrC, float, M * N) {
    rring_inner_preloop(wrC, float);
    rring_sync_writeonly(wrC);
    rring_inner_loop(wrC, i) {
      _inner_wrC[i] = 0;
    }
    rring_inner_wb(wrC);
  }
  rring_cleanup_writeonly(wrC);


  float *oC = (float *) aligned_alloc(4096, sizeof(float) * num_ele);

  __m256 alloca[4];
  __m256 ap;
  __m256 bv;
  __m256 mul;
  __m256i ones = _mm256_set1_epi32(1);

  uint64_t _rA = (uint64_t) A->_aligned_ptr;
  uint64_t _lA = (uint64_t) rbuf + (8192ULL);
  rring_init(rA, float, 2048 * 4, 8064, _lA, _rA);

  uint64_t _rB = (uint64_t) B->_aligned_ptr;
  uint64_t _lB = (uint64_t) rbuf + (8192ULL) + (256ULL << 20);
  rring_init(rB, float, 512 * 512 * 4, 1, _lB, _rB);

  rring_init(rC, float, 2048 * 4, 8064, _lC, _rC);

  float *fixB;
  rring_outer_loop(rB, float, N * K) {
    rring_prefetch(rB, 1);
    rring_inner_preloop(rB, float);
    rring_sync(rB);
    rring_inner_loop(rB, dead) {
      fixB = _inner_rB;
    }
  }
  rring_outer_loop_with(rC, M * N);
  rring_outer_loop(rA, float, M * K) {
    rring_prefetch_with(rA, rC, 4);
    rring_prefetch(rA, 4);

    rring_inner_preloop(rA, float);
    rring_inner_preloop(rC, float);

    rring_sync(rA);

    rring_inner_loop(rA, dead) {
      int64_t m = _t_rA * 4;
      for (int64_t n = 0; n < N; n += 8) {
        for (int64_t k = 0; k < K; k += 8) {
          // load C [4x8]
          for (int i = 0; i < 4; ++ i) {
            alloca[i] = _mm256_loadu_ps(pin2(_inner_rC, i, n));
            // alloca[i] = _mm256_loadu_ps(pin2(oC, m + i, n));
          }

          // C[4x8] += A[4x8] @ B[8x8]
          for (int i = 0; i < 8; i += 4) {
            for (int64_t mm = 0; mm < 4; ++ mm) {
              for (int64_t kk = k + i; kk < k + i + 4; ++kk) {
                ap = _mm256_broadcast_ss(pin2(_inner_rA, mm, kk));
                bv = _mm256_loadu_ps(pin2(fixB, kk, n));
                mul = _mm256_mul_ps(ap, bv);
                alloca[mm] = _mm256_add_ps(mul, alloca[mm]); 
              }
            }
          }
        }

        // Store C [4x8]
        for (int i = 0; i < 4; ++ i) {
          _mm256_maskstore_ps(pin2(oC, m + i, n), ones, alloca[i]);
        }
      }
      break;
    }
    rring_outer_loop_with_post(rC);
  }

  Tensor_float_2 output = make_tensor_float_2(oC, oshape);
  *C = output;
}

int main () {
  init_client();
  cache_init(); // use disagg_alloc

  int64_t shapeA[] = {M, K};
  float *bufA = read_tensor_float("/users/Zijian/raw_eth_pktgen/apps/bench-gemm/A.dat", shapeA, 2);

  int64_t shapeB[] = {K, N};
  float *bufB = read_tensor_float("/users/Zijian/raw_eth_pktgen/apps/bench-gemm/B.dat", shapeB, 2);

  // remotalize a and b
  uint64_t _rA = (uint64_t) _disagg_alloc(2, (256ULL << 20));
  uint64_t _lA = (uint64_t) rbuf + (8192ULL);
  rring_init(rA, float, 2048 * 4, 8064, _lA, _rA);

  uint64_t _rB = (uint64_t) _disagg_alloc(2, (1ULL << 20));
  uint64_t _lB = (uint64_t) rbuf + (8192ULL) + (256ULL << 20);
  rring_init(rB, float, 512 * 512 * 4, 1, _lB, _rB);

  printf("%ld, %ld\n", _rA, _rB);
  rring_outer_loop(rA, float, M * K) {
    rring_inner_preloop(rA, float);
    rring_sync_writeonly(rA);
    rring_inner_loop(rA, i) {
      size_t nth = _t_rA * _bn_rA + i;
      _inner_rA[i] = bufA[nth];
    }
    rring_inner_wb(rA);
  }
  rring_cleanup_writeonly(rA);

  rring_outer_loop(rB, float, N * K) {
    rring_inner_preloop(rB, float);
    rring_sync_writeonly(rB);
    rring_inner_loop(rB, i) {
      size_t nth = _t_rB * _bn_rB + i;
      _inner_rB[i] = bufB[nth];
    }
    rring_inner_wb(rB);
  }
  rring_cleanup_writeonly(rB);

  Tensor_float_2 A = make_tensor_float_2((float *)_rA, shapeA);
  Tensor_float_2 B = make_tensor_float_2((float *)_rB, shapeB);

  Tensor_float_2 C;
  int64_t shapeC[] = {M, N};
  float *C_truth = read_tensor_float("/users/Zijian/raw_eth_pktgen/apps/bench-gemm/C.dat", shapeC, 2);

  uint64_t start = microtime();
  _mlir_ciface_main_graph(&C, &A, &B);
  uint64_t end = microtime();
  printf("time: %.5f s\n", (float)(end-start)/1e6);

  return 0;
}
