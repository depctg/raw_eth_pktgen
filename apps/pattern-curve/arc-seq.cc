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

const uint64_t line_size = (2<<20);

const uint64_t c1_raddr = 0;
const uint64_t c1_size = (10ULL<< 20);
const int c1_slots = c1_size / line_size;

// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
using C1 = DirectCache<0,c1_raddr,0,c1_slots,line_size,0>;
using C1R = CacheReq<C1>;

const uint64_t eles = line_size / sizeof(arc_t);
const uint64_t n_blocks = N_node / eles;

void setup() {
  arc = (arc_t *) C1R::alloc(sizeof(arc_t) * M_arc);
  node = NULL;

  for (int j = 0; j < n_blocks; ++ j) {
    arc_t *arcb = C1R::get_mut<arc_t>(arc + j * eles);
    for (int i = 0; i < eles; ++ i) {
      arcb[i].head = node + nextRand();
    }
  }
  for (uint64_t i = 0; i < N_node; ++ i) {
    node_list[i] = i;
  }
  std::shuffle(node_list, node_list + N_node, g);
}

// TODO: node_t and arc_t
// add prefetch ?

const int n_ahead = 4;

// TODO: node_t and arc_t
void visit() {
  int offs[n_ahead+1];
  uint64_t tags[n_ahead+1];
  // prologue
  for (int i = 0; i < n_ahead; ++ i) {
    tags[i] = C1::Op::tag((uint64_t)(arc + i * eles));
    offs[i] = C1::select(tags[i]);
    C1R::request(offs[i], tags[i]);
  }

  for (int j = 0; j < n_blocks; ++ j) {
    int idx = j % (n_ahead + 1);

    // prefetch
    if (j < n_blocks - n_ahead) {
      int idxn = (j + n_ahead) % (n_ahead + 1);
      tags[idxn] = C1::Op::tag((uint64_t)(arc + (j+n_ahead) * eles));
      offs[idxn] = C1::select(tags[idxn]);
      C1R::request(offs[idxn], tags[idxn]);
    }

    // sync current
    auto &token = C1::Op::token(offs[idx]);
    token.add(Token::Dirty);
    poll_qid(C1::Value::qid, token.seq);
    arc_t *p = C1::Op::template paddr<arc_t>(offs[idx], (uint64_t)(arc + j * eles));

    for (int i = 0; i < eles; ++i) {
      p[i].tail = node + node_list[nextRand()];
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
  printf("node miss/hit = %lu, %lu, rate = %.5f\n", counters[3], counters[4], (float) counters[3] / (counters[3] + counters[4]));
  // check();
}


int main () {
  t0 = microtime();
  init_client();

  do_work();
  return 0;
}
