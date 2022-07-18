#include <stdio.h>
#include <float.h>
#include "util_disagg_aifm_naive.hpp"
#include "common.h"
#include "cache.h"

#define at(G, r, c) (G[r * (MAX_V) + c])
#define min_d(x, y) ((x) < (y) ? (x) : (y))
#define max_d(x, y) ((x) > (y) ? (x) : (y))

static const double MAX_D = DBL_MAX;

uint64_t gps = 4;
uint64_t gps_mask = ~(gps-1);

void heap_test(Graph *g) {
  MinHeap<MAX_V> *heap = init_min_heap<MAX_V>();
  for (int vid = 0; vid < g->V; ++vid) {
    if (g->l[vid].length > 0) {
      GraphNode *fn = access_graph_node_view(&g->l[vid].neighbours[0], 0);
      heap->array[vid] = new_heap_node(vid, fn->w);
    } else {
      heap->array[vid] = new_heap_node(vid, MAX_D);
    }
    heap->pos[vid] = vid;
  }
  heap->size = g->V;
  for (int r = parent_idx(heap->size - 1); r > -1; --r) {
    heapify(heap, r);
  }

  while (!is_heap_empty(heap)) {
    cache_token_t min_ptr = extract_min(heap);
    MinHeapNode *mn = access_heap_node_view(&min_ptr, 0);
    std::cout << mn->v << " " << mn->dist << std::endl;
  }
}

int main(int argc, char const *argv[])
{
	init(TRANS_TYPE_RC, argv[1]);
  cache_init();

  int total_v = 0;
  const char *dataf = argv[2];
  int redundant_data = atoi(argv[3]);
  int need_fake = atoi(argv[4]);

  Graph *g = init_graph(redundant_data, need_fake, dataf, &total_v);

  heap_test(g);
  return 0;
}
