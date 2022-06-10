#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <chrono>
#include <util.hpp>

#define at(G, r, c) (G[r * (MAX_V) + c])
#define min_d(x, y) ((x) < (y) ? (x) : (y))
#define max_d(x, y) ((x) > (y) ? (x) : (y))

using namespace std;
constexpr static double MAX_D = numeric_limits<double>::max();

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

  // mark s -> s = 0.0
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
  int total_v = 0;
  Graph *g = init_graph<true, true>(argv[1], total_v);
  double *solution = new double[total_v];
  dijkstra(g, 0, solution);
  
  cout << "Total number of vertices: " << g->V << endl;
  ofstream output("path_L.txt");
  for (int i = 0; i < total_v; ++ i)
    output << i << ", " << solution[i] << endl;
  output.close();

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
