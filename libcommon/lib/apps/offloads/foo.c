#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct Node {
  int v;
} Node;

typedef struct A {
  Node n;
  struct A *next;
} A;

typedef void (*rpc_service_t)(void *arg, void *ret);

// populate by offload obj
rpc_service_t *services;

extern void * deref_disagg_vaddr(uint64_t dvaddr);
// extern void * r_disagg_malloc(unsigned cache_id, size_t size);

static inline void visit_offloadable(A *head) {
  while (head) {
    A* dat = deref_disagg_vaddr((uint64_t) head);
    printf("%d\n", dat->n.v);
    head = dat->next;
  }
}

void service_wrapper_visit_offloadable(void *arg, void *ret) {
  A *head = * (A **) arg;
  visit_offloadable(head);
}

void init_rpc_services() {
  services = malloc(1 * sizeof(*services));
  services[0] = service_wrapper_visit_offloadable;
}