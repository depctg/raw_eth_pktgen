#ifndef SIDE_CHANNEL_H
#define SIDE_CHANNEL_H

#include <stdint.h>
#include <stdlib.h>

// max number of channels exit concurrently
#define MAX_NUM_CHANNEL 32

void channel_init();

enum {
  CHANNEL_LOAD = 0,
  CHANNEL_STORE
};

// channel grow downwards
// starting from rbuf + rbuf_size
// the creation and destroy of channel should resemble automatic resource
// otherwise we are in trouble managing the reservation

// the given `original_start_vaddr`
// is the "raw" vaddr from MLIR directly with cache id
unsigned channel_create(
  uint64_t original_start_vaddr, 
  uint64_t upper_bound, 
  uint64_t size_each, 
  unsigned num_slots, 
  unsigned batch, 
  unsigned dist, 
  int kind);

void * channel_access(unsigned channel, unsigned i);

void channel_destroy(unsigned channel);

#endif
