#include "common.h"
#include "ringbuf.h"

struct shared_data *ldata, *rdata;

int main(int argc, char **argv)
{
  init(TRANS_TYPE_SHM_EXECUTOR, argv[1]);
  execute_transfer(ldata, rdata);
  return 0;
}
