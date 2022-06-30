#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cache.h"
#include "common.h"

#define MAX_V 2000000
#define NO_EDGE 0.0


typedef struct GraphNode
{
  int dest;
  double w;
  struct GraphNode *next; // now represent addr in cache OR NULL
} GraphNode;

cache_t graph_node_cache;
uint64_t graph_node_size;
uint64_t graph_node_cls;
uint64_t free_graph_node_addr = sizeof(GraphNode);

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


GraphNode *access_graph_node_view(uint64_t cur_node_addr, int mut)
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

GraphNode* new_graph_node(int dest, double w, cache_token_t *t)
{
  uint64_t cur_addr = free_graph_node_addr;
  cache_token_t node_t = cache_request(graph_node_cache, cur_addr);

  // move free pointer
  free_graph_node_addr = align_next_free(free_graph_node_addr + sizeof(GraphNode), sizeof(GraphNode), graph_node_cls);

  cache_await(node_t);
  GraphNode *rel = (GraphNode *) ((char *) cache_access_mut(node_t) + cache_tag_mask(graph_node_cls, cur_addr));
  rel->dest = dest;
  rel->w = w;
  rel->next = NULL;
  *t = node_t;
  return (GraphNode *) ((void *) cur_addr);
}

// need to call twice if the given data is undirected
void add_edge(struct Graph *g, int s, int t, double w)
{
  cache_token_t new_node_token;
  GraphNode *n = new_graph_node(t, w, &new_node_token);
  // add to head of adj list
  // GraphNode *n_view = access_graph_node_view((uint64_t) n, 1);
  cache_await(new_node_token);
  GraphNode *n_view = (GraphNode *) ((char *) cache_access_mut(new_node_token) + cache_tag_mask(graph_node_cls, (uint64_t) n));
  dprintf("%d-%d %lf", s, n_view->dest, n_view->w);
  n_view->next = g->l[s].head;
  g->l[s].head = n;
}

struct Graph* init_graph(uint8_t redundant, uint8_t need_fake, const char *fpath, int *total_v)
{
  // init server must be done before this
  graph_node_cls = align_with_pow2(sizeof(GraphNode) * 16);
  graph_node_size = (64 << 10);
  graph_node_cache = cache_create_ronly(graph_node_size, graph_node_cls, (char *)rbuf);
  free_graph_node_addr = align_next_free(free_graph_node_addr, sizeof(GraphNode), graph_node_cls);

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

void inspect_graph(Graph *g)
{
  for (int i = 0; i < g->V; ++ i)
  {
    printf("--- %d : neighbours ---\n", i);
    for (GraphNode *cur_addr = g->l[i].head; cur_addr != NULL;)
    {
      GraphNode *cur_view = access_graph_node_view((uint64_t)cur_addr, 0);
      printf("%d %lf\n", cur_view->dest, cur_view->w);
      cur_addr = cur_view->next;
    }
  }
}

typedef struct MinHeapNode
{
  double dist;
  int v;
} MinHeapNode;

cache_t heap_node_cache;
uint64_t heap_node_size;
uint64_t heap_node_cls;
uint64_t free_heap_node_addr = sizeof(MinHeapNode);

#define heap_last(heap) ((heap)->array[(heap)->size - 1])
#define heap_idx(heap,idx) ((heap)->array[(idx)])
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

MinHeapNode* access_heap_node_view(uint64_t node_addr, int mut)
{
  cache_token_t node_t = cache_request(heap_node_cache, node_addr);
  cache_await(node_t);
  if (mut) {
    return (MinHeapNode *) ((char *) cache_access_mut(node_t) + cache_tag_mask(heap_node_cls, node_addr));
  } else {
    return (MinHeapNode *) ((char *) cache_access(node_t) + cache_tag_mask(heap_node_cls, node_addr));
  }
}

struct MinHeapNode* new_heap_node(int v, double dist, cache_token_t *t)
{
  uint64_t cur_addr = free_heap_node_addr;
  cache_token_t node_t = cache_request(heap_node_cache, cur_addr);

  // move free pointer
  free_heap_node_addr = align_next_free(free_heap_node_addr + sizeof(MinHeapNode), sizeof(MinHeapNode), heap_node_cls);

  cache_await(node_t);
  MinHeapNode *n = (MinHeapNode *) ((char *) cache_access_mut(node_t) + cache_tag_mask(heap_node_cls, cur_addr));
  n->v = v;
  n->dist = dist;
  *t = node_t;
  return (MinHeapNode *) cur_addr;
}

struct MinHeap *init_min_heap(int capacity)
{
  heap_node_cls = align_with_pow2(sizeof(MinHeapNode) * 8);
  heap_node_size = (64 << 20);
  heap_node_cache = cache_create_ronly(heap_node_size, heap_node_cls, (char *)rbuf + graph_node_size);
  free_heap_node_addr = align_next_free(free_heap_node_addr, sizeof(MinHeapNode), heap_node_cls);

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
  cache_token_t node_t;
  heap_last(heap) = new_heap_node(v, dist, &node_t);
  for (int i = heap->size - 1; 
       i;
       i = parent_idx(i))
  {
    MinHeapNode *parent_view = access_heap_node_view((uint64_t) heap_idx(heap, parent_idx(i)), 1);
    MinHeapNode *cur_view = access_heap_node_view((uint64_t) heap_idx(heap, i), 1);
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

void heapify(MinHeap *heap, int idx)
{
  int min, left, right;
  min = idx;
  left = left_child(idx);
  right = right_child(idx);

  // find min among left, right children and idx
  MinHeapNode *min_node = access_heap_node_view((uint64_t) heap_idx(heap, min), 0);
  if (left < heap->size)
  {
    MinHeapNode *left_node = access_heap_node_view((uint64_t) heap_idx(heap, left), 0);
    if (left_node->dist < min_node->dist) min = left;
  }
  min_node = access_heap_node_view((uint64_t) heap_idx(heap, min), 0);
  if (right < heap->size)
  {
    MinHeapNode *right_node = access_heap_node_view((uint64_t) heap_idx(heap, right), 0);
    if (right_node->dist < min_node->dist) min = right;
  }

  if (min != idx)
  {
    min_node = access_heap_node_view((uint64_t) heap_idx(heap, min), 0);
    MinHeapNode *idx_node = access_heap_node_view((uint64_t) heap_idx(heap, idx), 0);

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
  MinHeapNode *root_addr = heap->array[0];

  // replace with last node
  MinHeapNode *last_addr = heap_last(heap);
  heap->array[0] = last_addr;

  // update position of last node
  MinHeapNode *root_view = access_heap_node_view((uint64_t) root_addr, 0);
  MinHeapNode *last_view = access_heap_node_view((uint64_t) last_addr, 0);
  heap->pos[root_view->v] = heap->size - 1;
  heap->pos[last_view->v] = 0;

  // heapify
  heap->size --;
  heapify(heap, 0);
  return root_addr;
}

// reset dist value of a given vertex
void decrease_key(MinHeap *heap, int v, double dist)
{
  int i = heap->pos[v];
  MinHeapNode *i_node = access_heap_node_view((uint64_t) heap_idx(heap, i), 1);
  i_node->dist = dist;
  MinHeapNode *parent_node;
  while (i)
  {
    i_node = access_heap_node_view((uint64_t) heap_idx(heap, i), 0); 
    parent_node = access_heap_node_view((uint64_t) heap_idx(heap, parent_idx(i)), 0);
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

uint8_t heap_contains(MinHeap *heap, int v)
{
  return (heap->pos[v] < heap->size);
}