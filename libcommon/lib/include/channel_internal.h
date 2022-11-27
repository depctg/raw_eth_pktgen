#ifndef _CHANNEL_INTERNAL_H_
#define _CHANNEL_INTERNAL_H_

#include "side_channel.h"
#include <stdlib.h>
#include <stdint.h>

struct channel_internal {
  // data array [num * size]
  char * buf_base;
  // status array [num / batch]
  char * status;

  unsigned cache;
  uint64_t disagg_vaddr; // starting disagg vaddr
  uint64_t upper_bound; // access pattern: [0, 1, upper_bound)
  uint64_t max_reached;
  uint64_t size_each;
  unsigned prefetch_distance;
  unsigned num_slots; // capacity of this ringbuffer
  unsigned batch;
  int kind; // load / store
};

extern struct channel_internal channels[];

enum {
  CHANNEL_SLOT_IDLE = 0,
  CHANNEL_SLOT_SYNC,
  CHANNEL_SLOT_READY
};


#define channel_get_field(id,field) \
  (channels[id].field)
#define channel_get_data(id,i) \
  (channels[id].buf_base + (i) * channel_get_field(id,size_each))
#define channel_get_status(id,i) \
  (channels[id].status[i])


#endif
