#include <iostream>
#include <vector>
#include "memref.hpp"
#include "common.h"
#include "cache.h"
#include <string>
#include <pthread.h>

extern "C" {
size_t rdma_wrid_cnt = 1;
void rsync(size_t *r, size_t t);
void _mlir_ciface_run_task_0(StridedMemRefType<float,2> *sC,
  StridedMemRefType<float,2> *sA,
  StridedMemRefType<float,2> *sB);
void _mlir_ciface_run_task_1(StridedMemRefType<float,2> *sC,
  StridedMemRefType<float,2> *sA,
  StridedMemRefType<float,2> *sB);
}

static struct ibv_wc wcq[64];
void rsync(size_t *r, size_t t) {
    if (*r >= t)
        return;

    do {
        int n = ibv_poll_cq(cq, 16, wcq);
        for (int i = 0; i < n; i++) {
            // if (wc[i].status != 0) {
            //     printf("ERROR %d, %ld\n", wc[i].status, wc[i].wr_id);
            // }
            if (wcq[i].wr_id > *r)
                *r = wcq[i].wr_id;
        }
    } while (*r < t);
}

extern "C" void _mlir_ciface__driver(
  StridedMemRefType<float,2> *, 
  StridedMemRefType<float,2> *, 
  StridedMemRefType<float,2> *
);

extern "C" void _mlir_ciface_main_graph__0(
  StridedMemRefType<float,2> *, 
  StridedMemRefType<float,2> *, 
  StridedMemRefType<float,2> *
);

extern "C" void _mlir_ciface_main_graph__1(
  StridedMemRefType<float,2> *, 
  StridedMemRefType<float,2> *, 
  StridedMemRefType<float,2> *
);

constexpr int num_thread = 2;

struct T_pack {
  StridedMemRefType<float,2> *sC;
  StridedMemRefType<float,2> *sA;
  StridedMemRefType<float,2> *sB;
};

static T_pack ps[num_thread];
static pthread_t tids[num_thread];

void *task_0(void *data) {
  T_pack *p = (T_pack *)data;
  _mlir_ciface_main_graph__0(p->sC, p->sA, p->sB);
  return NULL;
}

void *task_1(void *data) {
  T_pack *p = (T_pack *)data;
  _mlir_ciface_main_graph__1(p->sC, p->sA, p->sB);
  return NULL;
}

void _mlir_ciface_run_task_0(StridedMemRefType<float,2> *sC,
  StridedMemRefType<float,2> *sA,
  StridedMemRefType<float,2> *sB) {
  ps[0].sC = sC;
  ps[0].sA = sA;
  ps[0].sB = sB;
  pthread_create(tids, NULL, task_0, ps);
}

void _mlir_ciface_run_task_1(StridedMemRefType<float,2> *sC,
  StridedMemRefType<float,2> *sA,
  StridedMemRefType<float,2> *sB) {
  ps[1].sC = sC;
  ps[1].sA = sA;
  ps[1].sB = sB;
  pthread_create(tids+1, NULL, task_1, ps+1);
}

int main() {
  init_client();
  cache_init(); // use disagg_alloc

  // C = A [64512x512] @ B[512x512]
  // get Mat A
  int64_t A_shape[] = {64512, 512};
  StridedMemRefType<float,2> sA[num_thread];
  for (int i = 0; i < num_thread; ++ i) {
    StridedMemRefType<float,2> _sA(A_shape, true);
    DynamicMemRefType<float> dA(_sA);
    // remotealize
    read_tensor("/users/Zijian/new_rt/apps/bench-matmul-mt/A.dat", dA, true);
    sA[i] = _sA;
  }

  // get B
  int64_t B_shape[] = {512, 512};
  StridedMemRefType<float,2> sB(B_shape);
  DynamicMemRefType<float> dB(sB);
  read_tensor("/users/Zijian/new_rt/apps/bench-matmul-mt/B.dat", dB);

  StridedMemRefType<float,2> output[num_thread];

  printf("After setup\n");

  uint64_t start_ns = getCurNs();
  _mlir_ciface__driver(output, sA, &sB);
  for (int i = 0; i < num_thread; ++ i) {
    pthread_join(tids[i], NULL);
  } 
  uint64_t end_ns = getCurNs();

  printf("Exec time %.6f s\n", (float)(end_ns - start_ns)/1e9); 
  for (int i = 0; i < num_thread; ++ i) {
    DynamicMemRefType<float> dO(output[i]);
    print_tensor<float>(dO, false);
  }
  return 0;
}