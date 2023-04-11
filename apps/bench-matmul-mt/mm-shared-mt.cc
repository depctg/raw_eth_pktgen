#include "common.h"
#include "tensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <immintrin.h>
#include "cache.hpp"
#include "rdmaop.hpp"
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

const uint64_t A_line = 4 * 512 * 4;
const uint64_t A_total_size = (320ULL << 20);
const int A_slots = A_total_size / A_line;
const uint64_t A_raddr = 0;

const uint64_t C_raddr = (4096ULL << 20);

using CA = DirectCache<0,A_raddr,0,A_slots,A_line,0>;
using CC = DirectCache<A_slots,C_raddr,(4096ULL<<20),A_slots,A_line,1>;

using CAR = CacheReq<CA,true>;
using CCR = CacheReq<CC,true>;


template<typename CR1, typename CR2>
void _mlir_ciface_main_graph(Tensor_float_2 *C, Tensor_float_2 *A, Tensor_float_2 *B) {
  int64_t oshape[] = {M, N};
  int64_t num_ele = 1;
  for (int i = 0; i < 2; ++ i)
    num_ele *= oshape[i];
  float *rC = (float *) CR1::alloc(sizeof(float) * num_ele);


  for (int64_t i = 0; i < M; i += 4) {
    float *lC = CR1::template get_mut<float>(pin2(rC, i, 0));
    memset(lC, 0, 8192);
    CR1::release(pin2(rC, i, 0));
  }

  __m256 alloca[4];
  __m256 ap;
  __m256 bv;
  __m256 mul;

  float *lB = B->_aligned_ptr;

  // printf("after get B\n");
  for (int64_t m = 0; m < M; m += 4) {
    float *lA = CR2::template get<float>(pin2(A->_aligned_ptr, m, 0));
    float *lC = CR1::template get_mut<float>(pin2(rC, m, 0));
    for (int64_t n = 0; n < N; n += 8) {
      for (int64_t k = 0; k < K; k += 8) {
        // load C [4x8]
        for (int i = 0; i < 4; ++ i) {
          alloca[i] = _mm256_loadu_ps(pin2(lC, i, n));
        }

        // C[4x8] += A[4x8] @ B[8x8]
        for (int i = 0; i < 8; i += 4) {
          for (int64_t mm = 0; mm < 4; ++ mm) {
            for (int64_t kk = k + i; kk < k + i + 4; ++kk) {
              ap = _mm256_broadcast_ss(pin2(lA, mm, kk));
              bv = _mm256_loadu_ps(pin2(lB, kk, n));
              mul = _mm256_mul_ps(ap, bv);
              alloca[mm] = _mm256_add_ps(mul, alloca[mm]); 
            }
          }
        }

        // Store C [4x8]
        for (int i = 0; i < 4; ++ i) {
          _mm256_storeu_ps(pin2(lC, i, n), alloca[i]);
        }
      }
    }
    CR1::release(pin2(rC, m, 0));
    CR2::release(pin2(A->_aligned_ptr, m, 0));
  }
  
  Tensor_float_2 output = make_tensor_float_2(rC, oshape);
  *C = output;
}

constexpr int num_thread = 5;
constexpr int N_input = 5; // multiple times of thread
constexpr int workload = N_input / num_thread;

struct T_pack {
  Tensor_float_2 *C;
  Tensor_float_2 *A;
  Tensor_float_2 *B;
};

template<typename CR1, typename CR2>
void *run(void *data) {
  T_pack *p = (T_pack *) data;
  for (int i = 0; i < workload; ++i) {
    _mlir_ciface_main_graph<CR1,CR2>(p->C + i, p->A + i, p->B);
  }
  return NULL;
}


void * rdma_poll_rountine(void *) {
  poll_all();
  return NULL;
}

int main () {
  // far mem setup
  init_client();
  pthread_t pool_t;
  pthread_create(&pool_t, NULL, rdma_poll_rountine, NULL);

  // assign workload
  float *bufA[N_input];
  float *rA[N_input];
  float *bufB;

  int64_t shapeA[] = {M, K};
  int64_t shapeB[] = {K, N};
  int64_t shapeC[] = {M, N};

  for (int i = 0; i < N_input; ++ i) {
    bufA[i] = read_tensor_float("/users/Zijian/new_runtime/cpy_new_rt/apps/bench-matmul-new/A.dat", shapeA, 2);
    rA[i] = (float *) CAR::alloc(sizeof(float) * M * K);
  }
  bufB = read_tensor_float("/users/Zijian/new_runtime/cpy_new_rt/apps/bench-matmul-new/B.dat", shapeB, 2);

  // push input
  for (int k = 0; k < num_thread; ++ k) {
    for (int64_t m = 0; m < M; m += 4) {
      float *wA = CAR::get_mut<float>(pin2(rA[k], m, 0));
      for (int64_t i = 0; i < 4; ++ i) {
        for (int64_t j = 0; j < N; ++ j) {
          *pin2(wA, i, j) = *pin2(bufA[k], m+i, j);
        }
      }
      CAR::release(pin2(rA[k], m, 0));
    }
  }

  printf("After push to remote\n");

  Tensor_float_2 A[num_thread][workload];
  Tensor_float_2 C[num_thread][workload];
  Tensor_float_2 B = make_tensor_float_2(bufB, shapeB);
  T_pack p[num_thread];
  for (int i = 0; i < num_thread; ++ i) {
    for (int j = 0; j < workload; ++ j) {
      A[i][j] = make_tensor_float_2(rA[i * workload + j], shapeA);
      C[i][j] = make_tensor_float_2(NULL, shapeC);
    }
    p[i].A = A[i];
    p[i].B = &B;
    p[i].C = C[i];
  }

  uint64_t start = microtime();
  pthread_t t[num_thread];
  for (int i = 0; i < num_thread; ++ i) {
    pthread_create(t+i, NULL, run<CAR,CCR>, p+i);
  }
  for (int i = 0; i < num_thread; ++ i) {
    pthread_join(t[i], NULL);
  }
  uint64_t end = microtime();
  printf("time: %.5f s\n", (float)(end-start)/1e6);

  for (int i = 0; i < 2; ++ i)
    printf("%ld\n", p->C[0].shapes[i]);
  // check_output_float(C._aligned_ptr, C_truth, shapeC, 2);

  // for (int i = 0; i < 2; ++ i)
  //   printf("%ld\n", C1.shapes[i]);
  // check_output_float(C1._aligned_ptr, C_truth, shapeC, 2);
  return 0;
}
