#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cache.h"
#include "common.h"

#define MAX_V 2000000
#define NO_EDGE 0.0

cache_t graph_node_cache;
cache_t heap_node_cache;
uint64_t graph_node_size;
uint64_t graph_node_cls;
uint64_t heap_node_size;
uint64_t heap_node_cls;

typedef struct GraphNode
{
  int dest;
  double w;
  struct GraphNode *next; // now represent addr in cache OR NULL
} GraphNode;

typedef struct AdjList
{
  // pointer to the head of the list
  struct GraphNode *head;
} AdjList;

typedef struct Graph
{
  int V;
  struct AdjList *l;
} Graph;

uint64_t free_node_addr = 0;

GraphNode *access_node_view(uint64_t cur_node_addr, int mut)
{
  cache_token_t node_t = cache_request(graph_node_cache, cur_node_addr);
  cache_await(node_t);
  // GraphNode *rel = malloc(sizeof(*rel));
  if (mut) {
    return (GraphNode *) ((char *) cache_access_mut(node_t) + cache_tag_mask(graph_node_cls, cur_node_addr));
  } else {
    return (GraphNode *) ((char *) cache_access(node_t) + cache_tag_mask(graph_node_cls, cur_node_addr));
  }
}

GraphNode* new_graph_node(int dest, double w)
{
  uint64_t cur_addr = free_node_addr;
  free_node_addr += sizeof(GraphNode);
  GraphNode *rel = access_node_view(cur_addr, 1);
  rel->dest = dest;
  rel->w = w;
  rel->next = NULL;
  return (GraphNode *) ((void *) cur_addr);
}

// need to call twice if the given data is undirected
void add_edge(struct Graph *g, int s, int t, double w)
{
  GraphNode *n = new_graph_node(t, w);
  // add to head of adj list
  uint64_t n_addr = (uint64_t) n; // addr in cache
  GraphNode *n_view = access_node_view(n_addr, 1);
  dprintf("%lu %d %lf", n_addr, n_view->dest, n_view->w);
  n_view->next = g->l[s].head;
  g->l[s].head = n;
}

struct Graph* init_graph(uint8_t redundant, uint8_t need_fake, const char *fpath, int *total_v)
{
  // init server must be done before this
  graph_node_cls = align_with_pow2(sizeof(GraphNode) * 16);
  graph_node_size = (64 << 20);
  graph_node_cache = cache_create_ronly(graph_node_size, graph_node_cls, (char *)rbuf);

  struct Graph *g = malloc(sizeof(*g));
  // need to be updated after read file
  g->V = MAX_V;
  g->l = malloc(sizeof(struct AdjList) * MAX_V);
  for (int i = 0; i < MAX_V; ++ i)
  {
    g->l[i].head = NULL;
  }

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
  g->V = vid_max;
  *total_v = vid_max;

  return g;
}


typedef struct MinHeapNode
{
  int v;
  double dist;
} MinHeapNode;


#define heap_last(heap) (heap->array[heap->size - 1])
#define parent_idx(i) ((i-1) / 2)
#define left_child(i) ((i << 1) + 1)
#define right_child(i) ((i << 1) + 2)

typedef struct MinHeap
{
  int size;
  int capacity;
  int *pos;
  struct MinHeapNode **array;
} MinHeap;

struct MinHeapNode* new_heap_node(int v, double dist)
{
  MinHeapNode *n = malloc(sizeof(*n));
  n->v = v;
  n->dist = dist;
  return n;
}

struct MinHeap *init_min_heap(int capacity)
{
  heap_node_cls = align_with_pow2(sizeof(MinHeapNode) * 20);
  heap_node_size = (64 << 20);
  heap_node_cache = cache_create_ronly(heap_node_size, heap_node_cls, (char *)rbuf + graph_node_size);

  MinHeap *heap = malloc(sizeof(*heap));
  heap->pos = malloc(sizeof(int) * capacity);
  heap->size = 0;
  heap->capacity = capacity;
  heap->array = malloc(sizeof(MinHeapNode *) * capacity);
  return heap;
}

void swap_heap_node(MinHeapNode **a, MinHeapNode **b)
{
  MinHeapNode *t = *a;
  *a = *b;
  *b = t;
}

void insert(MinHeap *heap, int v, double dist)
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
       i && heap->array[parent_idx(i)]->dist > heap->array[i]->dist; 
       i = parent_idx(i))
  {
    heap->pos[heap->array[parent_idx(i)]->v] = i;
    heap->pos[heap->array[i]->v] = parent_idx(i);
    swap_heap_node(&heap->array[i], &heap->array[parent_idx(i)]);
  }
}

void heapify(MinHeap *heap, int idx)
{
  int min, left, right;
  min = idx;
  left = left_child(idx);
  right = right_child(idx);

  if (left < heap->size && heap->array[left]->dist < heap->array[min]->dist)
    min = left;
  if (right < heap->size && heap->array[right]->dist < heap->array[min]->dist)
    min = right;
  
  if (min != idx)
  {
    MinHeapNode *min_node = heap->array[min];
    MinHeapNode *idx_node = heap->array[idx];

    heap->pos[min_node->v] = idx;
    heap->pos[idx_node->v] = min;

    swap_heap_node(&heap->array[min], &heap->array[idx]);
    heapify(heap, min);
  }
}

int is_heap_empty(MinHeap *heap) {
  return heap->size == 0;
}

MinHeapNode* extract_min(MinHeap *heap)
{
  if (is_heap_empty(heap)) return NULL;
  MinHeapNode *root = heap->array[0];

  // replace with last node
  MinHeapNode *last = heap_last(heap);
  heap->array[0] = last;

  // update position of last node
  heap->pos[root->v] = heap->size - 1;
  heap->pos[last->v] = 0;

  // heapify
  heap->size --;
  heapify(heap, 0);
  return root;
}

// reset dist value of a given vertex
void decrease_key(MinHeap *heap, int v, double dist)
{
  int i = heap->pos[v];
  heap->array[i]->dist = dist;

  while (i && heap->array[i]->dist < heap->array[parent_idx(i)]->dist)
  {
    heap->pos[heap->array[i]->v] = parent_idx(i);
    heap->pos[heap->array[parent_idx(i)]->v] = i;
    swap_heap_node(&heap->array[i], &heap->array[parent_idx(i)]);
    i = parent_idx(i);
  }
}

uint8_t heap_contains(MinHeap *heap, int v)
{
  return (heap->pos[v] < heap->size);
}