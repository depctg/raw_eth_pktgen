#include "util.h"
#include "common.h"
#include <stdio.h>
#include <float.h>

#define at(G, r, c) (G[r * (MAX_V) + c])
#define min_d(x, y) ((x) < (y) ? (x) : (y))
#define max_d(x, y) ((x) > (y) ? (x) : (y))

static const double MAX_D = DBL_MAX;

void dijkstra(Graph* graph, int src, double *solution)
{
  MinHeap *heap = init_min_heap(graph->V);
  for (int vid = 0; vid < graph->V; ++vid)
  {
    solution[vid] = MAX_D;
    heap->array[vid] = new_heap_node(vid, solution[vid]);
    heap->pos[vid] = vid;
  }
  heap->size = graph->V;
  for (int r = parent_idx(heap->size - 1); r > -1; --r) heapify(heap, r);

  solution[src] = 0.0;
  decrease_key(heap, src, 0.0);

  while (!is_heap_empty(heap))
  {
    MinHeapNode *min_node = extract_min(heap);
    int t= min_node->v;
    // traverse neighbours
    if (solution[t] >= MAX_D) break;
    for (GraphNode *cur = graph->l[t].head; cur != NULL; cur = cur->next)
    {
      int nid = cur->dest;
      if (heap_contains(heap, nid))
      {
        solution[nid] = min_d(solution[nid], cur->w + solution[t]);
        decrease_key(heap, nid, solution[nid]);
      }
    }
  }
}

int main(int argc, char const *argv[])
{
  uint64_t start = getCurNs();
  int total_v = 0;
  int redundant_data = atoi(argv[2]);
  int need_fake = atoi(argv[3]);
  Graph *g = init_graph(redundant_data, need_fake, argv[1], &total_v);
  // for (GraphNode *cur = g->l[1].head; cur != NULL; cur = cur->next)
  // {
  //   printf("%d %lf\n", cur->dest, cur->w);
  // }
  double *solution = malloc(sizeof(*solution) * g->V);
  dijkstra(g, 0, solution);

  uint64_t end = getCurNs();
  printf("ms: %lu\n", (end - start) / 1000);

  FILE *out = fopen("solution.txt", "w");
  for (int i = 0; i < total_v; ++ i)
  {
    fprintf(out, "%lf\n", solution[i]);
  }
  fclose(out);
  // MinHeap *h = init_min_heap(5);
  // for (int i = 0; i < 5; ++i) h->pos[i] = i;
  // h->array[0] = new_heap_node(0, 4.2);
  // h->array[1] = new_heap_node(1, 3.2);
  // h->array[2] = new_heap_node(2, 1.5);
  // h->array[3] = new_heap_node(3, 0.3);
  // h->array[4] = new_heap_node(4, 10.4);
  // h->size = 5;
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
  //   MinHeapNode *mn = extract_min(h);
  //   cout << mn->v << " " << mn->dist << endl;
  // }

  return 0;
}
