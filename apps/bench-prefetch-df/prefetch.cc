#include <cstdint>
#include <chrono>
#include <vector>
#include <iostream>
#include "rring.h"
#include "common.h"

using namespace std;

#define N (128ULL << 20)

static uint64_t seed = 0x23333;
static inline int nextRand(int M) {
  seed = ((seed * 7621) + 1) % M;

  // static std::mt19937 g(seed);
  // static std::uniform_int_distribution<int> dist(0, M-1);
  // seed = dist(g);

  // printf("%d\n", (int)r);
  return (int)seed;
  // return zipf(randrand, M) - 1;
}

static std::vector<uint64_t> v1;
static std::vector<int> v2;

static inline bool filter (uint64_t index, char flag) {
  if (flag == 'Y') {
    v1.push_back(index);
    return true;
  }
  return false;
}

const size_t eles = 1024 * 1024;


int main () {
  init_client();
  v1.reserve(N);
  v2.reserve(N);

rring_init(rvid_writer, 
  int, eles * sizeof(int), 32, (uint64_t) rbuf, 8192ULL);
rring_init(rflag_writer, 
  char, eles * sizeof(char), 32, (uint64_t) rbuf + (1ULL << 30), 8192ULL + (1ULL << 30));
rring_init(rids_writer, 
  uint64_t, eles * sizeof(uint64_t), 32, (uint64_t) rbuf + (2ULL << 30), 8192ULL + (2ULL << 30));

  rring_outer_loop(rvid_writer, int, N) {
    rring_inner_preloop(rvid_writer, int);
    rring_sync_writeonly(rvid_writer);
    rring_inner_loop(rvid_writer, j) {
        _inner_rvid_writer[j] = nextRand(127);
    }
    rring_inner_wb(rvid_writer);
  }
  rring_cleanup_writeonly(rvid_writer);

  rring_outer_loop(rflag_writer, char, N) {
  rring_inner_preloop(rflag_writer, char);
  rring_sync_writeonly(rflag_writer);
  rring_inner_loop(rflag_writer, j) {
      _inner_rflag_writer[j] = nextRand(N >> 1);
  }
  rring_inner_wb(rflag_writer);
  }
  rring_cleanup_writeonly(rflag_writer);

  rring_outer_loop(rids_writer, uint64_t, N) {
  rring_inner_preloop(rids_writer, uint64_t);
  rring_sync_writeonly(rids_writer);
  rring_inner_loop(rids_writer, j) {
      _inner_rids_writer[j] = nextRand(N);
  }
  rring_inner_wb(rids_writer);
  }
  rring_cleanup_writeonly(rids_writer);
  printf("data ready\n");

rring_init(rvid_reader, 
  int, eles * sizeof(int), 32, (uint64_t) rbuf, 8192ULL);
rring_init(rflag_reader, 
  char, eles * sizeof(char), 32, (uint64_t) rbuf + (1ULL << 30), 8192ULL + (1ULL << 30));
rring_init(rids_reader, 
  uint64_t, eles * sizeof(uint64_t), 32, (uint64_t) rbuf + (2ULL << 30), 8192ULL + (2ULL << 30));


  auto start = std::chrono::steady_clock::now(); 

    rring_outer_loop_with(rvid_reader, N);
    rring_outer_loop_with(rflag_reader, N);
    rring_outer_loop(rids_reader, size_t, N) {
        rring_prefetch_with(rids_reader, rflag_reader, 16);
        rring_prefetch_with(rids_reader, rvid_reader, 16);
        rring_prefetch(rids_reader, 16);

        rring_inner_preloop(rflag_reader, char);
        rring_inner_preloop(rvid_reader, int);
        rring_inner_preloop(rids_reader, size_t);

        rring_sync(rids_reader);

        rring_inner_loop(rids_reader, j) {
          char f = _inner_rflag_reader[j];
          int vid = _inner_rvid_reader[j];
          size_t idx = _inner_rids_reader[j];
          if (filter (idx, f))
            v2.push_back(vid);
        }
        rring_outer_loop_with_post(rvid_reader);
        rring_outer_loop_with_post(rflag_reader);
    }

  auto end = std::chrono::steady_clock::now(); 
  printf("Time %lu us\n", std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

  printf("filtered %lu\n", v1.size());
  return 0;
}