#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <algorithm>    // std::shuffle

#include "common.h"
#include "workload.hpp"
#include "cache.hpp"
#include "util.hpp"

// node
const uint64_t c1_line_size = (128);
const uint64_t c1_raddr = 0;
const uint64_t c1_size = (1000 << 20);
const int c1_slots = c1_size / c1_line_size;

// arc
const uint64_t c2_line_size = (2 << 20);
const uint64_t c2_raddr = 1024UL * 1024 * 1024;
const uint64_t c2_size = (24 << 20);
const int c2_slots = c2_size / c2_line_size;

// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
using C1 = SetAssocativeCache<0,c1_raddr,0,c1_slots,c1_line_size,0, /* num ways = */ 4>;
using C2 = DirectCache<c1_slots,c2_raddr,(1ULL<<30),c2_slots,c2_line_size,1>;

using C1R = CacheReq<C1>;
using C2R = CacheReq<C2>;

// CLS 2MB
const uint64_t eles = c2_line_size / sizeof(arc_t);
const uint64_t n_blocks = M_arc / eles;

void setup() {
  printf("size node: %lu\n", sizeof(node_t));
  printf("size arc:  %lu\n", sizeof(arc_t));
  node = (node_t *) C1R::alloc(sizeof(node_t) * N_node);
  arc = (arc_t *) C2R::alloc(sizeof(arc_t) * M_arc);

  for (int i = 0; i < N_node; ++ i) {
    node_t *nodei = C1R::get_mut<node_t>(node + i);
    nodei->number = i;
    nodei->firstin = arc + dist2(g);
    nodei->firstout = arc + dist2(g);
  }

  std::vector<uint64_t> node_list;
  node_list.reserve(N_node);
  for (uint64_t i = 0; i < N_node; ++ i) {
    node_list[i] = i;
  }
  shuffle(node_list.begin(), node_list.end(), std::default_random_engine(seed));

  for (int j = n_blocks - 1; j >= 0; j-- ) {
    // printf("%d, %lx\n", j, (uintptr_t) (arc + j*eles));
    arc_t *p = C2R::get_mut<arc_t>(arc + j * eles);
    for( int i = 0; i < eles; i++ ) { 
      p[i].tail = node + node_list[dist1(g)];
      p[i].head = node + node_list[dist1(g)];
    }
  }
}

const int n_ahead = 11;

// TODO: node_t and arc_t
void visit() {
  int offs[n_ahead+1];
  uint64_t tags[n_ahead+1];

  // prologue
  for (int i = 0; i < n_ahead; ++ i) {
    tags[i] = C2::Op::tag((uint64_t)(arc + i * eles));
    offs[i] = C2::select(tags[i]);
    auto &token = C2::Op::token(offs[i]);
    // if (!token.valid() || token.tag != tags[i]) {
      C2R::request(offs[i], tags[i]);
    // }
  }

  for (int j = 0; j < n_blocks; j++ ) {
    // arc_t *p = C2R::get_mut<arc_t>(arc + j * eles);
    int idx = j % (n_ahead + 1);

    // prefetch
    if (j < n_blocks - n_ahead) {
      int idxn = (j + n_ahead) % (n_ahead + 1);
      tags[idxn] = C2::Op::tag((uint64_t)(arc + (j+n_ahead) * eles));
      offs[idxn] = C2::select(tags[idxn]);

      auto &token = C2::Op::token(offs[idxn]);
      // if (!token.valid() || token.tag != tags[idxn]) {
        // printf("%d: pref in app -> [%lx], %lx, %d \n", j, (uint64_t)(arc + (j+n_ahead) * eles), tags[idxn], offs[idxn]);
        C2R::request(offs[idxn], tags[idxn]);
      // }
    }

    // sync current
    auto &token = C2::Op::token(offs[idx]);
    token.add(Token::Dirty);
    poll_qid(C2::Value::qid, token.seq);

    arc_t *p = C2::Op::template paddr<arc_t>(offs[idx], (uint64_t)(arc + j * eles));
    for( int i = 0; i < eles; i++ ) {
        arc_t *arci = p + i;
        node_t *node_tail = C1R::get_mut<node_t, 1>(arci->tail);
        arci->nextout = node_tail->firstout;
        node_tail->firstout = arc + j * eles + i;
        computation(arci, node_tail);

        node_t *node_head = C1R::get_mut<node_t, 1>(arci->head);
        arci->nextin = node_head->firstin;
        node_head->firstin = arc + j * eles + i;
        computation(arci, node_head);
    }
  }
}

uint64_t t0;
void do_work() {

  setup();
  printf("After setup\n");
  uint64_t start = microtime();
  visit();
  uint64_t end = microtime();
  printf("Visit start at: %.5f s\n", (start - t0)/1e6);
  printf("Exec time %.5f s\n", (end - start)/1e6);
  printf("Dont opt this %d\n", g_payload[5]);
  // check();
}

int main () {
  t0 = microtime();
  init_client();

  do_work();
  return 0;
}
