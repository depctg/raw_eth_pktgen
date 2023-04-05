
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

const int A_line = 4 * 512 * 4;
const int A_local_size = 67108864;
const int A_total_size = 136314880;
const int A_slots = A_local_size / A_line;
using CA0 = DirectCache<0*A_slots,0*A_total_size,0*A_local_size,A_slots,A_line,0>;
using CAR0 = CacheReq<CA0>;
using CA1 = DirectCache<2*A_slots,2*A_total_size,2*A_local_size,A_slots,A_line,2>;
using CAR1 = CacheReq<CA1>;
using CA2 = DirectCache<4*A_slots,4*A_total_size,4*A_local_size,A_slots,A_line,4>;
using CAR2 = CacheReq<CA2>;
using CA3 = DirectCache<6*A_slots,6*A_total_size,6*A_local_size,A_slots,A_line,6>;
using CAR3 = CacheReq<CA3>;
using CA4 = DirectCache<8*A_slots,8*A_total_size,8*A_local_size,A_slots,A_line,8>;
using CAR4 = CacheReq<CA4>;
using CA5 = DirectCache<10*A_slots,10*A_total_size,10*A_local_size,A_slots,A_line,10>;
using CAR5 = CacheReq<CA5>;
using CA6 = DirectCache<12*A_slots,12*A_total_size,12*A_local_size,A_slots,A_line,12>;
using CAR6 = CacheReq<CA6>;
using CC0 = DirectCache<1*A_slots,1*A_total_size,1*A_local_size,A_slots,A_line,1>;
using CCR0 = CacheReq<CC0>;
using CC1 = DirectCache<3*A_slots,3*A_total_size,3*A_local_size,A_slots,A_line,3>;
using CCR1 = CacheReq<CC1>;
using CC2 = DirectCache<5*A_slots,5*A_total_size,5*A_local_size,A_slots,A_line,5>;
using CCR2 = CacheReq<CC2>;
using CC3 = DirectCache<7*A_slots,7*A_total_size,7*A_local_size,A_slots,A_line,7>;
using CCR3 = CacheReq<CC3>;
using CC4 = DirectCache<9*A_slots,9*A_total_size,9*A_local_size,A_slots,A_line,9>;
using CCR4 = CacheReq<CC4>;
using CC5 = DirectCache<11*A_slots,11*A_total_size,11*A_local_size,A_slots,A_line,11>;
using CCR5 = CacheReq<CC5>;
using CC6 = DirectCache<13*A_slots,13*A_total_size,13*A_local_size,A_slots,A_line,13>;
using CCR6 = CacheReq<CC6>;

struct T_pack {
  Tensor_float_2 *C;
  Tensor_float_2 *A;
  Tensor_float_2 *B;
};

const static int64_t strides2[2] = {N, 1};
static inline float *pin2(float *buf, int64_t a, int64_t b) {
  return buf + a * strides2[0] + b;
}

template<typename CR1, typename CR2, int t>
void* _mlir_ciface_main_graph(void *data) {
  T_pack *p = (T_pack *) data;
  int64_t oshape[] = {M, N};
  int64_t num_ele = 1;
  for (int i = 0; i < 2; ++ i)
    num_ele *= oshape[i];

  // float *oC = (float *) aligned_alloc(4096, sizeof(float) * num_ele);
  float *rC = (float *) CR1::alloc(sizeof(float) * num_ele);
  for (int64_t i = 0; i < M; i += 4) {
    float *lC = CR1::template get_mut<float>(pin2(rC, i, 0));
    memset(lC, 0, 8192);
  }

  __m256 alloca[4];
  __m256 ap;
  __m256 bv;
  __m256 mul;

  float *lB = p->B->_aligned_ptr;

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

  float *rA0 = (float *) CAR0::alloc(sizeof(float) * M * K);
  float *rA1 = (float *) CAR1::alloc(sizeof(float) * M * K);
  float *rA2 = (float *) CAR2::alloc(sizeof(float) * M * K);
  float *rA3 = (float *) CAR3::alloc(sizeof(float) * M * K);
  float *rA4 = (float *) CAR4::alloc(sizeof(float) * M * K);
  float *rA5 = (float *) CAR5::alloc(sizeof(float) * M * K);
  float *rA6 = (float *) CAR6::alloc(sizeof(float) * M * K);

  for (int64_t m = 0; m < M; m += 4) {
    float *wA0 = CAR0::get_mut<float>(pin2(rA0, m, 0));
    float *wA1 = CAR1::get_mut<float>(pin2(rA1, m, 0));
    float *wA2 = CAR2::get_mut<float>(pin2(rA2, m, 0));
    float *wA3 = CAR3::get_mut<float>(pin2(rA3, m, 0));
    float *wA4 = CAR4::get_mut<float>(pin2(rA4, m, 0));
    float *wA5 = CAR5::get_mut<float>(pin2(rA5, m, 0));
    float *wA6 = CAR6::get_mut<float>(pin2(rA6, m, 0));
    for (int64_t i = 0; i < 4; ++ i) {
      for (int64_t j = 0; j < N; ++ j) {
        *pin2(wA0, i, j) = *pin2(bufA, m+i, j);
        *pin2(wA1, i, j) = *pin2(bufA, m+i, j);
        *pin2(wA2, i, j) = *pin2(bufA, m+i, j);
        *pin2(wA3, i, j) = *pin2(bufA, m+i, j);
        *pin2(wA4, i, j) = *pin2(bufA, m+i, j);
        *pin2(wA5, i, j) = *pin2(bufA, m+i, j);
        *pin2(wA6, i, j) = *pin2(bufA, m+i, j);
      }
    }
  }

  printf("After setup\n");
  
  Tensor_float_2 B = make_tensor_float_2(bufB, shapeB);
  Tensor_float_2 A0 = make_tensor_float_2(rA0, shapeA);
  Tensor_float_2 C0;
  T_pack p0 = {&C0, &A0, &B};
pthread_t t0;
  Tensor_float_2 A1 = make_tensor_float_2(rA1, shapeA);
  Tensor_float_2 C1;
  T_pack p1 = {&C1, &A1, &B};
pthread_t t1;
  Tensor_float_2 A2 = make_tensor_float_2(rA2, shapeA);
  Tensor_float_2 C2;
  T_pack p2 = {&C2, &A2, &B};
pthread_t t2;
  Tensor_float_2 A3 = make_tensor_float_2(rA3, shapeA);
  Tensor_float_2 C3;
  T_pack p3 = {&C3, &A3, &B};
pthread_t t3;
  Tensor_float_2 A4 = make_tensor_float_2(rA4, shapeA);
  Tensor_float_2 C4;
  T_pack p4 = {&C4, &A4, &B};
pthread_t t4;
  Tensor_float_2 A5 = make_tensor_float_2(rA5, shapeA);
  Tensor_float_2 C5;
  T_pack p5 = {&C5, &A5, &B};
pthread_t t5;
  Tensor_float_2 A6 = make_tensor_float_2(rA6, shapeA);
  Tensor_float_2 C6;
  T_pack p6 = {&C6, &A6, &B};
pthread_t t6;

  int64_t shapeC[] = {M, N};
  float *C_truth = read_tensor_float("/users/Zijian/new_runtime/cpy_new_rt/apps/bench-matmul-new/C.dat", shapeC, 2);

  uint64_t start = microtime();
  pthread_create(&t0, NULL, _mlir_ciface_main_graph<CCR0,CAR0,0>, &p0);
pthread_create(&t1, NULL, _mlir_ciface_main_graph<CCR1,CAR1,1>, &p1);
pthread_create(&t2, NULL, _mlir_ciface_main_graph<CCR2,CAR2,2>, &p2);
pthread_create(&t3, NULL, _mlir_ciface_main_graph<CCR3,CAR3,3>, &p3);
pthread_create(&t4, NULL, _mlir_ciface_main_graph<CCR4,CAR4,4>, &p4);
pthread_create(&t5, NULL, _mlir_ciface_main_graph<CCR5,CAR5,5>, &p5);
pthread_create(&t6, NULL, _mlir_ciface_main_graph<CCR6,CAR6,6>, &p6);
pthread_join(t0, NULL);
pthread_join(t1, NULL);
pthread_join(t2, NULL);
pthread_join(t3, NULL);
pthread_join(t4, NULL);
pthread_join(t5, NULL);
pthread_join(t6, NULL);
  uint64_t end = microtime();
  printf("time: %.5f s\n", (float)(end-start)/1e6);

  for (int i = 0; i < 2; ++ i)
    printf("%ld\n", C0.shapes[i]);
  return 0;
}


