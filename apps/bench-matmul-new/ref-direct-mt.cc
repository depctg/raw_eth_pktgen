#include "common.h"
#include "tensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <immintrin.h>
#include "cache.hpp"
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

const int A_line = 4 * 512 * 4;
const int A_total_size = (16 << 20);
const int A_slots = A_total_size / A_line;
const uint64_t A_raddr = 0;

const int B_line = 512 * 512 * 4;
const int B_total_size = B_line;
const int B_slots = 1;
const uint64_t B_raddr = (256ULL << 20);
const uint64_t C_raddr = (512ULL << 20);

using CA = DirectCache<0,A_raddr,0,A_slots,A_line,0>;
using CB = DirectCache<A_slots,B_raddr,(256<<20),B_slots,B_line,1>;
using CC = DirectCache<A_slots+1,C_raddr,(512<<20),A_slots,A_line,2>;
using CAR = CacheReq<CA>;
using CBR = CacheReq<CB>;
using CCR = CacheReq<CC>;

const uint64_t A1_raddr = (1024ULL << 20);
const uint64_t C1_raddr = (1536ULL << 20);
using CA1 = DirectCache<2*A_slots+1,A1_raddr,(1024<<20),A_slots,A_line,3>;
using CB1 = DirectCache<3*A_slots+1,B_raddr,(1536<<20),B_slots,B_line,4>;
using CC1 = DirectCache<3*A_slots+2,C1_raddr,(2048ULL<<20),A_slots,A_line,5>;
using CAR1 = CacheReq<CA1>;
using CBR1 = CacheReq<CB1>;
using CCR1 = CacheReq<CC1>;

struct T_pack {
  Tensor_float_2 *C;
  Tensor_float_2 *A;
  Tensor_float_2 *B;
};

template<typename CR1, typename CR2, typename CR3, int t>
void* _mlir_ciface_main_graph(void *data) {
  T_pack *p = (T_pack *) data;
  int64_t oshape[] = {M, N};
  int64_t num_ele = 1;
  for (int i = 0; i < 2; ++ i)
    num_ele *= oshape[i];

  float *rC = (float *) CR1::alloc(sizeof(float) * num_ele);
  for (int64_t i = 0; i < M; i += 4) {
    float *lC = CR1::template get_mut<float>(pin2(rC, i, 0));
    memset(lC, 0, 8192);
  }

  __m256 alloca[4];
  __m256 ap;
  __m256 bv;
  __m256 mul;

  float *lB = CR3::template get<float>(p->B->_aligned_ptr);

  // printf("after get B\n");
  for (int64_t m = 0; m < M; m += 4) {
    float *lA = CR2::template get<float>(pin2(p->A->_aligned_ptr, m, 0));
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
  }
  
  Tensor_float_2 output = make_tensor_float_2(rC, oshape);
  *(p->C) = output;
  printf("%d done\n", t);
  return NULL;
}


void * rdma_poll_rountine(void *) {
  poll_all();
  return NULL;
}

int main () {
  init_client();
  pthread_t pool_t;
  pthread_create(&pool_t, NULL, rdma_poll_rountine, NULL);

  int64_t shapeA[] = {M, K};
  float *bufA = read_tensor_float("/users/Zijian/new_runtime/cpy_new_rt/apps/bench-matmul-new/A.dat", shapeA, 2);

  int64_t shapeB[] = {K, N};
  float *bufB = read_tensor_float("/users/Zijian/new_runtime/cpy_new_rt/apps/bench-matmul-new/B.dat", shapeB, 2);

  float *rA = (float *) CAR::alloc(sizeof(float) * M * K);
  float *rB = (float *) CBR::alloc(sizeof(float) * K * N);

  float *rA1 = (float *) CAR1::alloc(sizeof(float) * M * K);

  // push input 0
  for (int64_t m = 0; m < M; m += 4) {
    float *wA = CAR::get_mut<float>(pin2(rA, m, 0));
    for (int64_t i = 0; i < 4; ++ i) {
      for (int64_t j = 0; j < N; ++ j) {
        *pin2(wA, i, j) = *pin2(bufA, m+i, j);
      }
    }
  }

  // push input 1
  for (int64_t m = 0; m < M; m += 4) {
    float *wA = CAR1::get_mut<float>(pin2(rA1, m, 0));
    for (int64_t i = 0; i < 4; ++ i) {
      for (int64_t j = 0; j < N; ++ j) {
        *pin2(wA, i, j) = *pin2(bufA, m+i, j);
      }
    }
  }

  // push weight
  float *wB = CBR::get_mut<float>(rB);
  for (int64_t i = 0; i < K * N; ++ i)
    wB[i] = bufB[i];

  printf("After push to remote\n");

  Tensor_float_2 A = make_tensor_float_2(rA, shapeA);
  Tensor_float_2 B = make_tensor_float_2(rB, shapeB);

  Tensor_float_2 A1 = make_tensor_float_2(rA1, shapeA);

  Tensor_float_2 C;
  Tensor_float_2 C1;
  int64_t shapeC[] = {M, N};
  float *C_truth = read_tensor_float("/users/Zijian/new_runtime/cpy_new_rt/apps/bench-matmul-new/C.dat", shapeC, 2);

  T_pack p = {&C, &A, &B};
  T_pack p1 = {&C1, &A1, &B};
  uint64_t start = microtime();
  pthread_t t;
  pthread_t t1;
  pthread_create(&t, NULL, _mlir_ciface_main_graph<CCR,CAR,CBR,0>, &p);
  pthread_create(&t1, NULL, _mlir_ciface_main_graph<CCR1,CAR1,CBR1,1>, &p1);
  pthread_join(t, NULL);
  pthread_join(t1, NULL);
  uint64_t end = microtime();
  printf("time: %.5f s\n", (float)(end-start)/1e6);

  for (int i = 0; i < 2; ++ i)
    printf("%ld\n", C.shapes[i]);
  // check_output_float(C._aligned_ptr, C_truth, shapeC, 2);

  // for (int i = 0; i < 2; ++ i)
  //   printf("%ld\n", C1.shapes[i]);
  // check_output_float(C1._aligned_ptr, C_truth, shapeC, 2);
  return 0;
}
