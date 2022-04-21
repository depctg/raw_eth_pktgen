#include <iostream>
#include <chrono>
#include "../common.h"
#include "../greeting.h"
#include "../cache.h"
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <memory>
#include <random>

constexpr static uint64_t localMem = 32 << 20;
constexpr static uint64_t kNumEntries = (16 << 20);
constexpr static uint64_t batch_size = (16384);
constexpr static uint64_t per_batch = batch_size / sizeof(uint64_t);
constexpr static uint64_t c_max_local = localMem / 2;
constexpr static uint64_t c_local_size = batch_size <= c_max_local ? batch_size : c_max_local;
constexpr static uint64_t per_tile = c_local_size / sizeof(uint64_t);
constexpr static uint64_t b_offset = kNumEntries * sizeof(uint64_t);
constexpr static uint64_t c_offset = 2 * kNumEntries * sizeof(uint64_t);
constexpr static size_t req_size = batch_size + sizeof(struct req);


void prepareAry(uint64_t *ary)
{
  std::random_device rd;
  std::mt19937_64 eng(rd());
  std::uniform_int_distribution<uint64_t> distr;
  for (uint64_t i = 0; i < kNumEntries; ++i)
  {
    // ary[i] = distr(eng);
    ary[i] = i;
  }
}

void evictAB(uint64_t *A, uint64_t *B, struct req *r)
{
  uint64_t *ab = (uint64_t *) (r+1);
  for (uint64_t i = 0; i < kNumEntries; i += per_batch / 2)
  { 
    for (uint64_t j = 0; j < per_batch; j += 2)
    {
      ab[j] = A[i+j/2];
      ab[j+1] = B[i+j/2];
    }
    r->addr = 2 * i * sizeof(uint64_t);
    r->size = batch_size;
    r->type = 0;
    send(r, req_size);
  } 
}

// fetch a batch from addr
uint64_t fetchAB(uint64_t addr, struct req *reqs, int buf_id)
{
  struct req *r = (struct req *) ((char *) reqs + buf_id * req_size);
  r->addr = addr;
  r->size = batch_size;
  r->type = 1;
  send_async(r, req_size);
  uint64_t wr_id = recv_async((char *) rbuf + buf_id * batch_size, batch_size);
  return wr_id;
}

using namespace std;

int main(int argc, char ** argv)
{
  init(TRANS_TYPE_RC, argv[1]);
  uint64_t *A = (uint64_t *) malloc(kNumEntries * sizeof(uint64_t));
  uint64_t *B = (uint64_t *) malloc(kNumEntries * sizeof(uint64_t));
  uint64_t *C = (uint64_t *) ((struct req *) sbuf + 1);
  prepareAry(A); prepareAry(B);
  
  uint64_t wr_id, wr_id_nxt;
  auto start = chrono::steady_clock::now();

  struct req *creq = (struct req *) sbuf;
  struct req *reqs = (struct req *) ((char *)sbuf + req_size);
  evictAB(A, B, reqs);

  int cur_c_i = 0;
  int c_tile_i = 0;
  const int num_buf = 1024;
  int buf_id = 0, buf_id_nxt = 0;
  // pre-send first request
  wr_id = fetchAB(0, reqs, buf_id);
  for (uint64_t i = 0; i < 2 * kNumEntries; i += per_batch)
  {
    // send next round
    buf_id_nxt = (buf_id + 1) % num_buf;
    if (i + per_batch < 2 * kNumEntries)
      wr_id_nxt = fetchAB((i + per_batch) * sizeof(uint64_t), reqs, buf_id_nxt);

    poll(wr_id);
    // process current batch
    uint64_t *AB_batch = (uint64_t *) ((char *)rbuf + buf_id * batch_size);
    for (int j = 0; j < per_batch / 2; j ++)
    {
      // cout << A_batch[j] << " " << B_batch[j] << endl;
      C[cur_c_i] = AB_batch[j*2] + AB_batch[j*2+1];
      cur_c_i ++;
      if (cur_c_i == per_tile)
      {
        creq->addr = c_offset + c_tile_i * c_local_size;
        creq->size = c_local_size;
        creq->type = 0;
        send(creq, req_size);
        cur_c_i = 0;
        c_tile_i ++;
      }
    }

    wr_id = wr_id_nxt;
    buf_id = buf_id_nxt;
  }
  if (cur_c_i > 0)
  {
    creq->addr = c_offset + c_tile_i * c_local_size;
    creq->size = cur_c_i * sizeof(uint64_t);
    creq->type = 0;
    send(creq, req_size);
  }

  auto end = chrono::steady_clock::now();
  std::cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;

  // assert
  buf_id_nxt = (buf_id + 1) % num_buf;
  struct req *r = (struct req *) ((char *) reqs + buf_id_nxt * req_size); 
  r->addr = c_offset;
	r->size = batch_size;
	r->type = 1;
	send_async(r, req_size);
	wr_id = recv_async((uint64_t *)rbuf + buf_id_nxt * per_batch, batch_size);
  buf_id = buf_id_nxt;
  for (int i = 0; i < kNumEntries; i += per_batch)
  {
    buf_id_nxt = (buf_id + 1) % num_buf;
    if (i + per_batch < kNumEntries) 
    {
      struct req *r = (struct req *) ((char *) reqs + buf_id_nxt * req_size);
      r->addr = c_offset + (i + per_batch) * sizeof(uint64_t);
      r->size = batch_size;
      r->type = 1;
      send_async(r, req_size);
      wr_id_nxt = recv_async((uint64_t *)rbuf + buf_id_nxt * per_batch, batch_size);
    }
    poll(wr_id);
    uint64_t *c_remote = (uint64_t *)rbuf + buf_id * per_batch;
    for (int j = 0; j < per_batch; ++ j)
      assert(c_remote[j] == A[i+j] + B[i+j]);
      // cout << c_remote[j] << endl;
      // if (c_remote[j] != A[i+j] + B[i+j])
      //   cout << c_remote[j] << " " << A[i+j] + B[i+j] << endl;
    wr_id = wr_id_nxt;
    buf_id = buf_id_nxt;
  }
}