#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "workload.hpp"
#include "cache.hpp"

const int slots = 1024;
const uint64_t c1_raddr = 0;
const uint64_t c2_raddr = 1024UL * 1024 * 1024;
// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
using C1 = DirectCache<0,c1_raddr,0,slots,1024,0>;
using C2 = DirectCache<1024,c2_raddr,C1::Value::bytes,slots,1024,1>;

using C1R = CacheReq<C1>;
using C2R = CacheReq<C2>;

void setup() {
  node = (node_t *) C1R::alloc(sizeof(node_t) * N_node);
  arc = (arc_t *) C2R::alloc(sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    node_t *nodei = C1R::get_mut<node_t>(node + i);
    // get_mus:
    // 1. int id = C1R::request(vaddr)
    // 1.5 optional 
    //    poll_qid(C1::Value::qid, C1::Op::token(id).seq);
    //    C1::Op::token(id).add(Token::Dirty); mut
    // 2. <T> *ptr = C1::Op::paddr<T>(id, vaddr)
    nodei->number = -i;
    nodei->firstin = arc + nextRand(M_arc);
    nodei->firstout = arc + nextRand(M_arc);
  }

  for (int i = 0; i < M_arc; ++ i) {
    arc_t *arci =  C1R::get_mut<arc_t>(arc + i);
    arci->tail = node + nextRand(N_node);
    arci->head = node + nextRand(N_node);
  }
}

// TODO: node_t and arc_t
void visit() {
  for( int i = 0; i < M_arc; i++ )
  {
    arc_t *arci = (arc_t *) C1R::get_mut<arc_t>(arc + i);
    node_t *node_tail = (node_t *) C2R::get_mut<node_t>(arci->tail);

    arci->nextout = node_tail->firstout;
    node_tail->firstout = arc + i;

    node_t *node_head = C2R::get_mut<node_t>(arci->head);

    arci->nextin = node_tail->firstin;
    node_head->firstin = arc + i;
  }
}

void check() {
  uint64_t check_sum = 0;
  for (int i = 0; i < M_arc; ++ i) {
    arc_t *arci = C1R::get<arc_t>(arc + i);
    node_t *node_tail = C2R::get<node_t>(arci->tail);
    check_sum += node_tail->number;

    arci =  C1R::get<arc_t>(arc + i);
    node_t *node_head = C2R::get<node_t>(arci->head);
    check_sum += node_head->number;
  }
  printf("Checksum = %lu\n", check_sum);
}

void do_work() {
  setup();

  uint64_t start = microtime();
  visit();
  uint64_t end = microtime();

  printf("Exec time %lu us\n", end - start);
  // check();
}

int main () {
  init_client();

  do_work();
  return 0;
}
