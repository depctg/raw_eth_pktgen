#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "bzlib.h"
#include <chrono>
#include <iostream>

typedef unsigned char uchar;

#define min(a,b) ((a) < (b) ? (a) : (b))

#define blockSize100k 1
#define verbosity 0
#define workFactor 30
#define batch_size (4 << 10)

using namespace std;
using namespace std::chrono;
int main(int argc, char **argv)
{
  char *compress_target = argv[1];

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
  uint64_t comp_ms = 0;
  uint64_t num_comp = 0;
  auto start = steady_clock::now();
  // read into local mem
  uint64_t n_ibuf;
  uchar *ibuf = (uchar *)malloc(sizeof(uchar) * batch_size);
  while (1) {
    n_ibuf = fread(ibuf, sizeof(uchar), batch_size, inStr);
    if (n_ibuf > 0) {
      auto zip_start = high_resolution_clock::now();
      BZ2_bzWrite( &bzerr, bzf, ibuf, n_ibuf);
      auto zip_end = high_resolution_clock::now();
      if (n_ibuf == batch_size) {
        comp_ms += duration_cast<microseconds>(zip_end - zip_start).count();
        num_comp ++;
      }
    }
    if (n_ibuf < batch_size) break;
  }

  auto end = chrono::steady_clock::now();

  BZ2_bzWriteClose64(&bzerr, bzf, 0,
                     &nbytes_in_lo32, &nbytes_in_hi32,
                     &nbytes_out_lo32, &nbytes_out_hi32);

  ret = fflush ( outStr );
  fclose ( outStr );
  outStr = inStr = NULL;

  cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
  cout << "zip-single-ms: " << comp_ms / num_comp << endl;
  return 1;
}