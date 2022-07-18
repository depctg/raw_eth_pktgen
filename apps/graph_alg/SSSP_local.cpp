#include <stdio.h>
#include <float.h>
#include "util_disagg_aifm_naive.hpp"
#include "common.h"
#include "cache.h"

#define min_d(x, y) ((x) < (y) ? (x) : (y))
#define max_d(x, y) ((x) > (y) ? (x) : (y))

static const double MAX_D = DBL_MAX;

void dijkstra(Graph* graph, int src, double *solution)
{
  MinHeap<MAX_V> *heap = init_min_heap<MAX_V>();
  for (int vid = 0; vid < graph->V; ++vid)
  {
    solution[vid] = MAX_D;
    heap->array[vid] = new_heap_node(vid, solution[vid]);
    heap->pos[vid] = vid;
  }
  heap->size = graph->V;
  printf("After heap init\n");
  
  for (int r = parent_idx(heap->size - 1); r > -1; --r) {
    heapify(heap, r);
  }
  printf("After heapify\n");

  // barrier();
  solution[src] = 0.0;
  decrease_key(heap, src, 0.0);
  MinHeapNode *alast_n = access_heap_node_view(&heap_last(heap), 0);

  while (!is_heap_empty(heap))
  {
    cache_token_t min_ptr = extract_min(heap);
		MinHeapNode *min_node = access_heap_node_view(&min_ptr, 0);
    int t = min_node->v;
    // printf("%d %f\n", t, min_node->dist);
    // traverse neighbours
    if (solution[t] >= MAX_D) break;
    for (int i = 0; i < graph->l[t].length; ++ i)
    {
      GraphNode *cur_view = access_graph_node_view(&graph->l[t].neighbours[i], 0);
      int nid = cur_view->dest;
      if (heap_contains(heap, nid))
      {
        solution[nid] = min_d(solution[nid], cur_view->w + solution[t]);
        decrease_key(heap, nid, solution[nid]);
      }
    }
  }
}

int main(int argc, char const *argv[])
{
	init(TRANS_TYPE_RC, argv[1]);
  cache_init();
  const char *dataf = argv[2];
  int redundant_data = atoi(argv[3]);
  int need_fake = atoi(argv[4]);

  uint64_t start = getCurNs();

  int total_v = 0;
  Graph *graph = init_graph(redundant_data, need_fake, dataf, &total_v);
  printf("After graph init\n");
  // inspect_graph(graph);

  double *solution = new double[graph->V];
  dijkstra(graph, 0, solution);

  uint64_t end = getCurNs();
  printf("us: %lu\n", (end - start) / 1000);

  FILE *out = fopen("solution_disagg.txt", "w");
  for (int i = 0; i < total_v; ++ i)
    fprintf(out, "%lf\n", solution[i]);
  fclose(out);

  // MinHeap<5> *h = init_min_heap<5>();
  // for (int i = 0; i < 5; ++i) h->pos[i] = i;
  // h->array[0] = new_heap_node(0, 4.2);
  // h->array[1] = new_heap_node(1, 3.2);
  // h->array[2] = new_heap_node(2, 1.5);
  // h->array[3] = new_heap_node(3, 0.3);
  // h->array[4] = new_heap_node(4, 10.4);
  // h->size = 5;
  // // for (int i = 0; i < 5; ++ i) {
  // //   MinHeapNode *m = access_heap_node_view(&h->array[i], 0);
  // //   std::cout << m->v << " " << m->dist << std::endl;
  // // }

  // for (int i = parent_idx(h->size - 1); i > -1; i--)
  // {
  //   heapify(h, i);
  // }

  // // insert(h, 0, 4.2);
  // // insert(h, 1, 3.2);
  // // insert(h, 2, 1.5);
  // // insert(h, 3, 0.3);
  // // insert(h, 4, 10.4);
  // decrease_key(h, 0, 0.0);
  // while (!is_heap_empty(h))
  // {
  //   cache_token_t min_ptr = extract_min(h);
  //   MinHeapNode *mn = access_heap_node_view(&min_ptr, 0);
  //   std::cout << mn->v << " " << mn->dist << std::endl;
  // }

  return 0;
}
