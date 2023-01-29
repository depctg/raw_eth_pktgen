#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "cache.h"
#include "offload.h"

typedef struct Node {
  int v;
} Node;

typedef struct A {
  Node n;
  struct A *next;
} A;

A **glob;

A *expand(A* node, int v) {
  A* new_node = (A*) _disagg_alloc(2, sizeof(A));
  cache_token_t tk = cache_request((intptr_t) new_node);
  A* dat = cache_access_nrtc_mut(&tk);
  dat->n.v = v;
  dat->next = NULL;

  cache_token_t tk1 = cache_request((intptr_t) node);
  A* dat1 = cache_access_nrtc_mut(&tk1);
  dat1->next = new_node;
  return new_node;
}

A *offloaded_expand(A* node, int v) {
  * (A **) offload_arg_buf = node;
  * (int *) ((A **) offload_arg_buf + 1) = v;
  return * (A**) call_offloaded_service(1, sizeof(A*) + sizeof(int), sizeof(A*));
}

void visit(int n) {
  A *head = glob[n];
  printf("struct chasing from %d\n", n);
  while (head) {
    cache_token_t tk = cache_request((intptr_t) head);
    A* dat = cache_access_nrtc(&tk);
    printf("%d\n", dat->n.v);
    head = dat->next;
  }
}

void visit_offloadable(A *head) {
  while (head) {
    cache_token_t tk = cache_request((intptr_t) head);
    A* dat = cache_access_nrtc(&tk);
    printf("%d\n", dat->n.v);
    head = dat->next;
  }
}

void offloaded_visit_offloadable(A *head) {
  * (A **) offload_arg_buf = head;
  call_offloaded_service(0, sizeof(head), 0);
}

int main(int argc, char **argv) {
  init_client();
  cache_init();
  int l = atoi(argv[1]);
  int n = atoi(argv[2]);

  glob = malloc(sizeof(A*) * l);
  // A *prev = _disagg_stack_alloc(sizeof(*prev));
  A *prev = _disagg_alloc(2, sizeof(*prev));
  for (int i = 0; i < l; ++i) {
    // A *an = expand(prev, i);
    A *an = offloaded_expand(prev, i);
    glob[i] = an;
    prev = an;
  }

  // visit(n);
  // visit_offloadable(glob[n]);
  offloaded_visit_offloadable(glob[n]);
  return 0;
}