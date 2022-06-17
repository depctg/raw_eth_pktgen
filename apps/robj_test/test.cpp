#include <iostream>
#include <stdlib.h>
#include "robj.h"


struct msg_full {
  remote_msg_header header;
  uint8_t payload[1 << 7];
};

using namespace std;
int main(int argc, char **argv) {
  cout << sizeof(remote_msg_header) << endl;
  cout << sizeof(msg_full) << endl;
  size_t DSS[2] = { sizeof(uint32_t), sizeof(uint64_t) };

  void *req_header_buf = malloc(4 << 20);
  void *recv_buf = malloc(30 << 20);
  init_manager(DSS, req_header_buf, recv_buf);

  disagg_ptr ptr = alloc_obj_n(0, 10);
  inspect_ptr(&ptr);

  uint32_t *obj = (uint32_t *) lookup_ptr(&ptr);
  obj[0] = 32;
  obj[1] = 64;
  cout << *((uint32_t *) ptr.addr + 0) << endl;
  cout << *((uint32_t *) ptr.addr + 1) << endl;

  evict(&ptr);
}