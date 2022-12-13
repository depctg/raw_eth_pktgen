#include <cstdint>
#include <chrono>
#include <vector>
#include <iostream>

using namespace std;

#define N (128ULL << 20)

char *flag;
int *vendorID;
uint64_t *index;

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

int main () {
  flag = (char *) malloc(sizeof(char) * N);
  for (size_t i = 0; i < N; ++ i)
    flag[i] = nextRand(127);
  vendorID = (int *) malloc(sizeof(int) * N);
  for (size_t i = 0; i < N; ++ i)
    vendorID[i] = nextRand(N >> 1);
  index = (uint64_t *) malloc(sizeof(uint64_t) * N);
  for (size_t i = 0; i < N; ++ i)
    index[i] = nextRand(N);

  v1.reserve(N);
  v2.reserve(N);

  auto start = std::chrono::steady_clock::now(); 

  for (size_t i = 0; i < N; ++ i) {
    char f = flag[i];
    int vid = vendorID[i];
    size_t idx = index[i];
    if (filter (idx, f))
      v2.push_back(vid);
  }
  auto end = std::chrono::steady_clock::now(); 
  printf("Time %lu us\n", std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

  printf("filtered %lu\n", v1.size());
  return 0;
}