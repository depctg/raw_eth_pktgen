#include <stdio.h>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include "cache.h"
#include "common.h"
#include "helper.h"

#define MAX_V (2 << 20)
#define NO_EDGE 0.0
#define LOG_ACCESS_LAT

#ifdef LOG_ACCESS_LAT
uint64_t graph_access_lat = 0;
uint64_t heap_access_lat = 0;
#endif

/* cache setup */

// GraphNode pointer -> cache token
struct GraphNode
{
  double w;
  int dest;

  GraphNode(int dest, double w): w(w), dest(dest) {}
};
cache_t graph_node_cache;
uint64_t graph_node_size = (4U << 20);
uint64_t graph_node_cls = align_with_pow2(sizeof(GraphNode) * 8);
uint64_t free_graph_node_addr = align_next_free(sizeof(GraphNode) /* 0 for null*/, sizeof(GraphNode), graph_node_cls);

// MinHeapNode pointer -> cache token
struct MinHeapNode
{
  double dist;
  int v;

  MinHeapNode() {}
  MinHeapNode(double dist, int v): dist(dist), v(v) {}
};
cache_t heap_node_cache;
uint64_t heap_node_size = (2U << 20);
uint64_t heap_node_cls = align_with_pow2(sizeof(MinHeapNode) * 4);
uint64_t free_heap_node_addr = align_next_free(sizeof(MinHeapNode), sizeof(MinHeapNode), heap_node_cls);


struct AdjList
{
  std::vector<cache_token_t>neighbours;
  int length;

  AdjList(): length(0) {}
};

struct Graph
{
  int V;
  AdjList *l;
  Graph(): V(MAX_V), l(new AdjList[MAX_V]) {}
  ~Graph() { delete[] l; }
};

GraphNode *access_graph_node_view(cache_token_t *node_t, int mut)
{
  if (mut)
    return (GraphNode *) cache_access_mut(node_t);
  else
    return (GraphNode *) cache_access(node_t);
}

cache_token_t new_graph_node(int dest, double w)
{
  uint64_t cur_addr = free_graph_node_addr;
  cache_token_t node_t;
  cache_request(graph_node_cache, cur_addr, &node_t);

  // move free pointer
  free_graph_node_addr = align_next_free(free_graph_node_addr + sizeof(GraphNode), sizeof(GraphNode), graph_node_cls);

  GraphNode *rel = access_graph_node_view(&node_t, 1);
  rel->dest = dest;
  rel->w = w;
  return node_t;
}

// need to call twice if the given data is undirected
void add_edge(struct Graph *g, int s, int t, double w)
{
  cache_token_t new_node_token = new_graph_node(t, w);
#ifdef DEBUG
  GraphNode *n_view = access_graph_node_view(&new_node_token, 0);
  dprintf("%d-%d %lf", s, n_view->dest, n_view->w);
#endif
  g->l[s].neighbours.push_back(new_node_token);
  g->l[s].length ++;
}

Graph* init_graph(uint8_t redundant, uint8_t need_fake, const char *fpath, int *total_v)
{
  printf("Graph CLS: %lu B\n", graph_node_cls);
  printf("Graph CS %f MB\n", graph_node_size / (double)(1<<20));
  // init server must be done before this
  graph_node_cache = cache_create(graph_node_size, graph_node_cls, (char *)rbuf);

  Graph *g = new Graph();

  FILE *fptr = fopen(fpath, "r");
  // read from file
  if (fptr == NULL)
  {
    printf("Couldn't open file %s\n", fpath);
    exit(1);
  }
  int eid, sid, tid, vid_max;
  eid = sid = tid = 0;
  int prev_sid = -1;
  int inter = 0;
  vid_max = -1;
  double w;

  while(1)
  {
    if (!need_fake) 
    {
      // iss >> eid >> sid >> tid >> w;
      if (fscanf(fptr, "%d %d %d %lf\n", &eid, &sid, &tid, &w) == EOF) break;
    }
    else
    {
      // iss >> sid >> tid;
      if (fscanf(fptr, "%d %d\n", &sid, &tid) == EOF) break;
      eid ++;
      inter = (prev_sid == sid) ? inter + 1 : 0;
      w = (++inter) / (double)100;
    }
    if (sid < MAX_V && tid < MAX_V)
    {
      // if (sid < 10) cout << line << endl;
      add_edge(g, sid, tid, w);
      vid_max = (vid_max > sid) ? vid_max : sid;

      if (!redundant) 
      {
        add_edge(g, tid, sid, w);
        vid_max = (vid_max > tid) ? vid_max : tid;
      } 
    }
  }
  fclose(fptr);
  g->V = vid_max + 1;
  *total_v = vid_max + 1;

  return g;
}

void inspect_graph(Graph *g)
{
  for (int i = 0; i < g->V; ++ i)
  {
    printf("--- %d : neighbours ---\n", i);
    for (int j = 0; j < g->l[i].length; ++ j)
    {
      cache_token_t t = g->l[i].neighbours[j];
      line_header *lh = (line_header *)(uintptr_t)(t.head_addr);
      GraphNode *cur_view = access_graph_node_view(&t, 0);
      printf("%d %lf\n", cur_view->dest, cur_view->w);
    }
  }
}

#define heap_last(heap) ((heap)->array[(heap)->size-1])
#define heap_idx(heap,idx) ((heap)->array[(idx)])
#define parent_idx(i) ((i-1) / 2)
#define left_child(i) ((i << 1) + 1)
#define right_child(i) ((i << 1) + 2)

template<int N>
struct MinHeap
{
  int size;
  int capacity;
  int *pos;
  cache_token_t array[N];

  MinHeap(): size(0), capacity(N), pos(new int[N]) {}
};

MinHeapNode* access_heap_node_view(cache_token_t *node_t, int mut)
{
  if (mut) {
    return (MinHeapNode *) cache_access_mut(node_t);
  } else {
    return (MinHeapNode *) cache_access(node_t);
  }
}

cache_token_t new_heap_node(int v, double dist)
{
  uint64_t cur_addr = free_heap_node_addr;
  cache_token_t node_t;
  cache_request(heap_node_cache, cur_addr, &node_t);

  // move free pointer
  free_heap_node_addr = align_next_free(free_heap_node_addr + sizeof(MinHeapNode), sizeof(MinHeapNode), heap_node_cls);

  MinHeapNode *n = access_heap_node_view(&node_t, 1);
  n->v = v;
  n->dist = dist;
  return node_t;
}

template<int capacity>
struct MinHeap<capacity> *init_min_heap()
{
  printf("MinHeap CLS: %lu B\n", heap_node_cls);
  printf("MinHeap CS %f MB\n", heap_node_size / (double)(1<<20));
  heap_node_cache = cache_create(heap_node_size, heap_node_cls, (char *)rbuf + (graph_node_size/graph_node_cls) * (sizeof(line_header) + graph_node_cls));

  MinHeap<capacity> *heap = new MinHeap<capacity>();
  return heap;
}

// swap pointer of HeapNode ptr (cache_token_t)
static inline void swap_heap_node(cache_token_t *a, cache_token_t *b)
{
  cache_token_t t = *a;
  *a = *b;
  *b = t;
}

template<int capacity>
void insert(MinHeap<capacity> *heap, int v, double dist)
{
  if (heap->size == heap->capacity)
  {
    printf("Not enough space in Heap");
    exit(1);
  }

  // insert at the end
  heap->size ++;
  heap_last(heap) = new_heap_node(v, dist);
  for (int i = heap->size - 1; 
       i;
       i = parent_idx(i))
  {
    MinHeapNode *parent_view = access_heap_node_view(&heap_idx(heap, parent_idx(i)), 0);
    MinHeapNode *cur_view = access_heap_node_view(&heap_idx(heap, i), 0);
    if (parent_view->dist > cur_view->dist)
    {
      heap->pos[parent_view->v] = i;
      heap->pos[cur_view->v] = parent_idx(i);
      swap_heap_node(&heap->array[i], &heap->array[parent_idx(i)]);
    } 
    else
    {
      break;
    }
  }
}

template<int capacity>
void heapify(MinHeap<capacity> *heap, int idx)
{
  int min, left, right;
  min = idx;
  left = left_child(idx);
  right = right_child(idx);

  // find min among left, right children and idx
  if (left < heap->size)
  {
    MinHeapNode *min_node = access_heap_node_view(&heap_idx(heap,min), 0);
    MinHeapNode *left_node = access_heap_node_view(&heap_idx(heap,left), 0);
    if (left_node->dist < min_node->dist) min = left;
  }
  if (right < heap->size)
  {
    MinHeapNode *min_node = access_heap_node_view(&heap_idx(heap,min), 0);
    MinHeapNode *right_node = access_heap_node_view(&heap_idx(heap,right), 0);
    if (right_node->dist < min_node->dist) min = right;
  }
  if (min != idx)
  {
    MinHeapNode *min_node = access_heap_node_view(&heap_idx(heap, min), 0);
    MinHeapNode *idx_node = access_heap_node_view(&heap_idx(heap, idx), 0);

    heap->pos[min_node->v] = idx;
    heap->pos[idx_node->v] = min;

    swap_heap_node(&heap->array[min], &heap->array[idx]);
    heapify(heap, min);
  }
}

template<int capacity>
int is_heap_empty(MinHeap<capacity> *heap) {
  return heap->size == 0;
}

template<int capacity>
cache_token_t extract_min(MinHeap<capacity> *heap)
{
  if (is_heap_empty(heap)) return {};
  cache_token_t root = heap->array[0];
  MinHeapNode *root_view = access_heap_node_view(&root, 0);

  // replace with last node
  cache_token_t last = heap_last(heap);
  MinHeapNode *last_view = access_heap_node_view(&last, 0);
  heap->array[0] = last;

  // update position of last node
  heap->pos[root_view->v] = heap->size - 1;
  heap->pos[last_view->v] = 0;

  // heapify
  heap->size --;
  heapify(heap, 0);
  return root;
}

template<int capacity>
// reset dist value of a given vertex
void decrease_key(MinHeap<capacity> *heap, int v, double dist)
{
  int i = heap->pos[v];
  MinHeapNode *i_node = access_heap_node_view(&heap_idx(heap, i), 1);
  i_node->dist = dist;
  while (i)
  {
    i_node = access_heap_node_view(&heap_idx(heap, i), 0); 
    MinHeapNode *parent_node = access_heap_node_view(&heap_idx(heap, parent_idx(i)), 0);
    if (i_node->dist < parent_node->dist)
    {
      heap->pos[i_node->v] = parent_idx(i);
      heap->pos[parent_node->v] = i;
      swap_heap_node(&heap->array[i], &heap->array[parent_idx(i)]);
      i = parent_idx(i);
    }
    else
    {
      break;
    }
  }
}

template<int capacity>
uint8_t heap_contains(MinHeap<capacity> *heap, int v)
{
  return (heap->pos[v] < heap->size);
}