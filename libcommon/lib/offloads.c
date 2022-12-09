#include "remote_pool.h"

#define MAX_SERVICES 1024

static rpc_service_t __services[1024];

rpc_service_t *services = &__services[0];

void __service_sum(void *, void*);
void __step1_unique(void *, void*);
void __step2_passage_count_by_vid(void *, void *);

void init_rpc_services() {
    __services[0] = __service_sum;
    //
}

