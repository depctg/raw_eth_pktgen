#include <iostream>
#include <chrono>
#include <stdlib.h>
#include "common.h"
#include "greeting.h"
#include "cache.h"
#include "replacement.h"
#include "clock.hpp"
#include <random>
#include <assert.h>

constexpr static uint64_t max_size = 1 << 19;
constexpr static uint64_t cache_line_size = 1 << 10;
constexpr static bool need_slim = true;
constexpr static int seed = 1234;
constexpr static int N = 256;

using namespace std;

mt19937 gen(seed);
// dense matrix
uniform_int_distribution<> d(1, N);

void rand_fill(uint64_t *m) {
  for (int i = 0; i < N*N; ++ i)
    m[i] = d((gen));
}

void print_mat(uint64_t *m, string const &label) {
  cout << label << endl;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++ j) {
      cout << m[i * N + j] << " ";
    }
    cout << endl;
  }
}

uint64_t *transpose(uint64_t *m) {
  uint64_t *tm = (uint64_t *) malloc(sizeof(uint64_t) * N * N);
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++ j) {
      tm[i * N + j] = m[j * N + i];
    }
  }
  return tm;
}

uint64_t mat_addr_uint(uint64_t *m, int i, int j) {
  return reinterpret_cast<uint64_t>((void *) (m + i * N + j));
}

// C = A dot B
int main(int argc, char **argv) {
  uint64_t mat_size = sizeof(uint64_t) * N * N;
  uint64_t *A = (uint64_t *) aligned_alloc(sizeof(uint64_t), mat_size);
  uint64_t *B = (uint64_t *) aligned_alloc(sizeof(uint64_t), mat_size);
  uint64_t *C = (uint64_t *) aligned_alloc(sizeof(uint64_t), mat_size);
  uint64_t *ref_C = (uint64_t *) aligned_alloc(sizeof(uint64_t), mat_size);
  rand_fill(A);
  rand_fill(B);
  if (need_slim)
    B = transpose(B);
  memset(C, 0, mat_size);
  memset(ref_C, 0, mat_size);

  auto local_start = chrono::steady_clock::now();
  if (need_slim) {
    for (int i = 0; i < N; ++ i) {
      for (int j = 0; j < N; ++ j) {
        for (int k = 0; k < N; ++ k) {
          ref_C[i * N + j] += A[i * N + k] * B[j * N + k];
        }
      }
    }
  } else {
    for (int i = 0; i < N; ++ i) {
      for (int j = 0; j < N; ++ j) {
        for (int k = 0; k < N; ++ k) {
          ref_C[i * N + j] += A[i * N + k] * B[k * N + j];
        }
      }
    }
  }
  auto local_end = chrono::steady_clock::now();
  cout << "local matmul ms: " << chrono::duration_cast<chrono::microseconds>(local_end - local_start).count() << endl;
  // print_mat(A, "A");
  // print_mat(B, "B");
  // print_mat(ref_C, "ref C");
	init(TRANS_TYPE_RC, argv[1]);
  // CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf, future_depth, add_to_OPT, touch_OPT, pop_OPT);
  CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf, 0, add_to_LRU, touch_LRU, pop_LRU);
  // CacheTable *cache = createCacheTable(max_size, cache_line_size, sbuf, rbuf, N * N, add_to_LFU, touch_LFU, pop_LFU);
  remote_write_n(cache, mat_addr_uint(A, 0, 0), A, mat_size);
  remote_write_n(cache, mat_addr_uint(B, 0, 0), B, mat_size);
  remote_write_n(cache, mat_addr_uint(C, 0, 0), C, mat_size);
  cout << "Start remote version" << endl;
  auto start = chrono::steady_clock::now();
  for (int i = 0; i < N; ++ i) {
    for (int j = 0; j < N; ++ j) {
      uint64_t Cij = *(uint64_t *) cache_access(cache, mat_addr_uint(C, i, j));
      for (int k = 0; k < N; ++ k) {
        uint64_t Aik = *(uint64_t *) cache_access(cache, mat_addr_uint(A, i, k));
        uint64_t Bkj;
        if (need_slim)
          Bkj = *(uint64_t *) cache_access(cache, mat_addr_uint(B, j, k));
        else
          Bkj = *(uint64_t *) cache_access(cache, mat_addr_uint(B, k, j));
        Cij += Aik * Bkj;
        // cout << Cij << endl;
      }
      cache_write(cache, mat_addr_uint(C, i, j), &Cij);
    }
  }

  auto end = chrono::steady_clock::now();
  std::cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << ", miss rate: " << ((float)cache->misses / (float)cache->accesses) * 100 << std::endl;
  // cout << "result C: " << endl;
  for (int i = 0; i < N; ++ i) {
    for (int j = 0; j < N; ++ j) {
      uint64_t Cij = *(uint64_t *) cache_access(cache, mat_addr_uint(C, i, j));
      // cout << Cij << " ";
      assert(Cij == ref_C[i * N + j]);
    }
    // cout << endl;
  }
  return 0;
}