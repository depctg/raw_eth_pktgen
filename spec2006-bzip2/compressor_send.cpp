#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "common.h"
#include "greeting.h"
#include "bzlib.h"
#include <chrono>
#include <iostream>

typedef unsigned char uchar;

#define min(a,b) ((a) < (b) ? (a) : (b))

#define blockSize100k 1
#define verbosity 0
#define workFactor 30
#define batch_size (1 << 20)

using namespace std;
using namespace std::chrono;
int main(int argc, char **argv)
{
  char *compress_target = argv[1];
  init(TRANS_TYPE_RC, argv[2]);

  char compressed_dest_fname[1024];
  memset(compressed_dest_fname, '\0', sizeof(compressed_dest_fname));
  sprintf(compressed_dest_fname, "%s.bz2", compress_target);
  printf("Compressing %s to %s\n", compress_target, compressed_dest_fname);

  FILE *inStr = fopen(compress_target, "rb");
  FILE *outStr = fopen(compressed_dest_fname, "wb");

  // Init bzip routine
  int bzerr, ret;
  uint32_t nbytes_in_lo32, nbytes_in_hi32;
  uint32_t nbytes_out_lo32, nbytes_out_hi32;
  BZFILE *bzf = BZ2_bzWriteOpen(&bzerr, outStr, blockSize100k, verbosity, workFactor);

  // read file to remote memory
  uint64_t total_ibuf = 0;
  uint64_t n_ibuf = 0;
  int max_send_bufs = CQ_NUM_DESC / 2;
  int cur_send_posts = 0;
  uint64_t send_wr_id = 0;
  uint64_t req_size = batch_size + sizeof(struct req);

  auto start = steady_clock::now();
  while (1) {
    if (cur_send_posts == max_send_bufs) {
      poll_opcode(send_wr_id, IBV_WC_SEND);
      cur_send_posts = 0;
    }
    uchar *req_dat = (uchar*) sbuf + cur_send_posts * req_size;
    memset(req_dat, 0, req_size);
    struct req *r = (struct req *) req_dat;
    r->addr = total_ibuf * sizeof(uchar);
    r->type = 0;
    r->size = batch_size;

    n_ibuf = fread(r+1, sizeof(uchar), batch_size, inStr);
    if (n_ibuf > 0) {
      // write to remote
      r->size = n_ibuf * sizeof(uchar);
      send_wr_id = send_async(r, req_size);
      total_ibuf += n_ibuf;
      cur_send_posts ++;
    }
    if (n_ibuf < batch_size) break;
  } 
  if (cur_send_posts > 0)
    poll_opcode(send_wr_id, IBV_WC_SEND);
  printf("Target file size: %" PRId64 "\n", total_ibuf);
  auto read_complete = steady_clock::now();

  uint64_t zip_ms = 0;
  uint64_t fetch_ms = 0;
  uint64_t recv_wr_id = 0, next_wr_id = 0;
  const int num_bufs = 4;
  int cur_buf_id = 0;
  struct req *reqs = (struct req *) sbuf;
  reqs[cur_buf_id].addr = 0;
  reqs[cur_buf_id].type = 1;
  reqs[cur_buf_id].size = batch_size;
  send_async(reqs + cur_buf_id, req_size);
  recv_wr_id = recv_async(rbuf, batch_size);

  uint64_t compressed_n = 0;
  while (compressed_n < total_ibuf)
  {
    int next_buf_id = (cur_buf_id + 1) % num_bufs;
    if (compressed_n + batch_size < total_ibuf) {
      reqs[next_buf_id].addr = compressed_n + batch_size;
      reqs[next_buf_id].type = 1;
      reqs[next_buf_id].size = batch_size;
      send_async(reqs + next_buf_id, req_size);
      next_wr_id = recv_async((uchar *) rbuf + next_buf_id * batch_size, batch_size);
    }
    auto fetch_start = high_resolution_clock::now();
    poll_opcode(recv_wr_id, IBV_WC_RECV);
    auto fetch_end = high_resolution_clock::now();
    fetch_ms += duration_cast<microseconds>(fetch_end - fetch_start).count();
    BZ2_bzWrite(&bzerr, bzf, (uchar *) rbuf + cur_buf_id * batch_size, min(total_ibuf - compressed_n, batch_size));
    auto zip_end = high_resolution_clock::now();
    zip_ms += duration_cast<microseconds>(zip_end - fetch_end).count();

    compressed_n += batch_size;
    recv_wr_id = next_wr_id;
    cur_buf_id = next_buf_id;
  }

  auto end = chrono::steady_clock::now();

  BZ2_bzWriteClose64(&bzerr, bzf, 0,
                     &nbytes_in_lo32, &nbytes_in_hi32,
                     &nbytes_out_lo32, &nbytes_out_hi32);

  ret = fflush ( outStr );
  fclose ( outStr );
  outStr = inStr = NULL;

  cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
  cout << "read ms: " << duration_cast<microseconds>(read_complete - start).count() << endl;
  cout << "zip ms: " << zip_ms << endl;
  cout << "fetch ms: " << fetch_ms << endl;
  return 1;
}