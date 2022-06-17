#ifndef __R_OBJ_POOL__
#define __R_OBJ_POOL__

#include <stdint.h>
#include <stdlib.h>
#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Stuff for data exchange with the remote side */
#define max_inflight 64
#define payload_Bsize (1 << 4)

enum {
  REQ_WRITE = 0,
  REQ_READ,
};

typedef union{
  uint8_t header_buf[24];
  struct {
    uint64_t obj_id; 
    uint64_t offset; // size_t offset to the starting address of obj_id
    uint8_t type: 4;
    uint64_t p_Bsize: 60; // valid size_t length of payload
  };
} remote_msg_header;

struct local_msg_header {
  uint64_t offset;
  uint64_t p_Bsize;
};

/* Basic remoteable pointer structure and methods */
enum ptr_status {
  IDLE = 0,
  SYNC,
  READY,
};

enum ptr_flags {
  VALID = 1 << 0,
  DIRTY = 1 << 1,
  PRESENCE = 1 << 2,
};

size_t *DS_LIST;

void init_manager(size_t *dss, void *sbuf_reserve, void *rbuf_reserve);

typedef struct disagg_ptr {
  uint64_t obj_id;
  uint64_t addr;
  uint64_t mem_size; 
  uint8_t ds_id;
  uint8_t status;
  uint8_t flags;
} disagg_ptr;

// request memory space for n objects
// return the starting address
disagg_ptr alloc_obj_n(uint8_t ds_id, uint64_t n);

void inspect_ptr(disagg_ptr *ptr);
void dealloc(disagg_ptr *ptr);
void evict(disagg_ptr *ptr);
void prefetch(disagg_ptr *ptr);
void await(disagg_ptr *ptr);
void sync_obj(disagg_ptr *ptr);
void *lookup_ptr(disagg_ptr *ptr);

#ifdef __cplusplus
}
#endif
#endif