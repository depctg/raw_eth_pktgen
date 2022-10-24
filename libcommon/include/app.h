#ifndef _APP_H_
#define _APP_H_

#ifdef __cplusplus
extern "C"
{
#endif 

#include <stdlib.h>
#include <stdint.h>

#define PAYLOAD_LIMIT (4 << 10)

/* op_code spec */
enum {
  // Cache related code
  CACHE_REQ_WRITE = 0,
  CACHE_REQ_READ,
  CACHE_REQ_EVICT,
  CACHE_REQ_MEMCOPY,
  CACHE_REQ_MEMMOVE,

  // Always push at the last one
  FUNC_CALL_BASE
};

typedef struct cache_req {
  uint64_t tag;
  uint64_t tag2: 48;
  uint8_t cache_id; 
  uint8_t unused;
} cache_req_t;

typedef struct call_req {
  uint64_t procedure_id;
  uint32_t arg_size;
  uint16_t ret_size;
  uint16_t unued;
} call_req_t;

typedef struct RemoteRequest {
  uint8_t op_code;
  union {
    cache_req_t cache_r_header;
    call_req_t call_r_header;
  };
} RPC_rr_t;

typedef struct RemoteRequestFULL {
  RPC_rr_t rr;
  char data_seg_base[PAYLOAD_LIMIT];
} RPC_rrf_t;

#ifdef __cplusplus
}
#endif
#endif 
