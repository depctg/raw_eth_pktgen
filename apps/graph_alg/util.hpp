#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <chrono>
#include "cache.h"

#define MAX_V 2000000
#define NO_EDGE 0.0

using namespace std;

struct GraphNode
{
  int dest;
  double w;
  struct GraphNode *next;
};

struct AdjList
{
  // pointer to the head of the list
  struct GraphNode *head;
};

struct Graph
{
  int V;
  struct AdjList *l;
};

struct GraphNode* new_graph_node(int dest, double w)
{
  GraphNode *rel = new GraphNode;
  rel->dest = dest;
  rel->w = w;
  rel->next = NULL;
  return rel;
}

// need to call twice if the given data is undirected
void add_edge(struct Graph *g, int s, int t, double w)
{
  GraphNode *n = new_graph_node(t, w);
  // add to head of adj list
  n->next = g->l[s].head;
  g->l[s].head = n;
}

template <bool redundant, bool need_fake, cache_t cache>
struct Graph* init_graph(const char *fpath, int &total_v)
{
  Graph *g = new Graph;
  // need to be updated after read file
  g->V = MAX_V;
  g->l = new AdjList[MAX_V];
  for (int i = 0; i < MAX_V; ++ i)
  {
    g->l[i].head = NULL;
  }

  // read from file
  ifstream f(fpath);
  if (!f.is_open())
  {
      cout << "Failed to open " << fpath << endl;
  }
  int eid, sid, tid, vid_max;
  eid = sid = tid = 0;
  int prev_sid = -1;
  int inter = 0;
  vid_max = -1;
  double w;

  for (string line; getline(f, line);)
  {
    istringstream iss(line);
    if (!need_fake)
      iss >> eid >> sid >> tid >> w;
    else
    {
      iss >> sid >> tid;
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
  f.close();
  g->V = vid_max;
  total_v = vid_max;

  return g;
}


struct MinHeapNode
{
  int v;
  double dist;
};

#define heap_last(heap) (heap->array[heap->size - 1])
#define parent_idx(i) ((i-1) / 2)
#define left_child(i) ((i << 1) + 1)
#define right_child(i) ((i << 1) + 2)

struct MinHeap
{
  int size;
  int capacity;
  int *pos;
  struct MinHeapNode **array;
};

struct MinHeapNode* new_heap_node(int v, double dist)
{
  MinHeapNode *n = new MinHeapNode;
  n->v = v;
  n->dist = dist;
  return n;
}

struct MinHeap *init_min_heap(int capacity)
{
  MinHeap *heap = new MinHeap;
  heap->pos = new int[capacity];
  heap->size = 0;
  heap->capacity = capacity;
  heap->array = new MinHeapNode*[capacity];
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
    cout << "Not enough space in Heap" << endl;
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

bool is_heap_empty(MinHeap *heap) {
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

bool heap_contains(MinHeap *heap, int v)
{
  if (heap->pos[v] < heap->size) return true;
  return false;
}