#include "tensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <immintrin.h>
#include "common.h"
#include <pthread.h>
#include "util.hpp"

#define M 64512
#define K 512
#define N 512

DECL_TENSOR(float, 2);
DEF_TENSOR_UTILS(float)

const static int64_t strides2[2] = {N, 1};
static inline float *pin2(float *buf, int64_t a, int64_t b) {
  return buf + a * strides2[0] + b;
}

struct T_pack {
  Tensor_float_2 *C;
  Tensor_float_2 *A;
  Tensor_float_2 *B;
};

void * _mlir_ciface_main_graph(void * data) {
  T_pack *p = (T_pack *) data;
  int64_t oshape[] = {M, N};
  int64_t num_ele = 1;
  for (int i = 0; i < 2; ++ i)
    num_ele *= oshape[i];

  float *iA = p->A->_aligned_ptr;
  float *iB = p->B->_aligned_ptr;
  float *oC = (float *) aligned_alloc(4096, sizeof(float) * num_ele);
  memset(oC, 0, sizeof(float) * num_ele);

  __m256 alloca[4];
  __m256 ap;
  __m256 bv;
  __m256 mul;

  for (int64_t m = 0; m < M; m += 4) {
    for (int64_t n = 0; n < N; n += 8) {
      for (int64_t k = 0; k < K; k += 8) {
        // load C [4x8]
        for (int i = 0; i < 4; ++ i) {
          alloca[i] = _mm256_loadu_ps(pin2(oC, m + i, n));
        }

        // C[4x8] += A[4x8] @ B[8x8]
        for (int i = 0; i < 8; i += 4) {
          for (int64_t mm = m; mm < m + 4; ++ mm) {
            for (int64_t kk = k + i; kk < k + i + 4; ++kk) {
              ap = _mm256_broadcast_ss(pin2(iA, mm, kk));
              bv = _mm256_loadu_ps(pin2(iB, kk, n));
              mul = _mm256_mul_ps(ap, bv);
              alloca[mm-m] = _mm256_add_ps(mul, alloca[mm-m]); 
            }
          }
        }

        // Store C [4x8]
        for (int i = 0; i < 4; ++ i) {
          _mm256_storeu_ps(pin2(oC, m + i, n), alloca[i]);
        }
      }
    }
  }
  Tensor_float_2 output = make_tensor_float_2(oC, oshape);
  *(p->C) = output;
  return NULL;
}

const int num_thread = 8;

int main () {
  float *bufA[num_thread];
  float *bufB;

  int64_t shapeA[] = {M, K};
  int64_t shapeB[] = {K, N};

  for (int i = 0; i < num_thread; ++ i) {
    bufA[i] = read_tensor_float("/users/Zijian/new_runtime/cpy_new_rt/apps/bench-matmul-new/A.dat", shapeA, 2);
  }
  bufB = read_tensor_float("/users/Zijian/new_runtime/cpy_new_rt/apps/bench-matmul-new/B.dat", shapeB, 2);

  Tensor_float_2 A[num_thread];
  Tensor_float_2 C[num_thread];
  Tensor_float_2 B = make_tensor_float_2(bufB, shapeB);
  T_pack p[num_thread];

  for (int i = 0; i < num_thread; ++ i) {
    A[i] = make_tensor_float_2(bufA[i], shapeA);
    p[i].A = A + i;
    p[i].B = &B;
    p[i].C = C + i;
  }


  int64_t shapeC[] = {M, N};
  float *C_truth = read_tensor_float("/users/Zijian/new_runtime/cpy_new_rt/apps/bench-matmul-new/C.dat", shapeC, 2);

  pthread_t t[num_thread];
  uint64_t start = microtime();
  for (int i = 0; i < num_thread; ++ i) {
    pthread_create(t+i, NULL, _mlir_ciface_main_graph, p+i);
  }
  for (int i = 0; i < num_thread; ++ i) {
    pthread_join(t[i], NULL);
  }
  uint64_t end = microtime();
  printf("time: %.5f s\n", (float)(end-start)/1e6);

  for (int i = 0; i < 2; ++ i)
    printf("%ld\n", p[0].C->shapes[i]);
  check_output_float(p[0].C->_aligned_ptr, C_truth, shapeC, 2);  
  return 0;
}
