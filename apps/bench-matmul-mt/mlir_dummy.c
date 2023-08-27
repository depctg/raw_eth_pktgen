#include <pthread.h>
#include <stdio.h>

typedef struct Pack {
  int *a;
  int *b; 
} Pack;

void foo(int *a, int *b) {
  printf("%d\n", *a + *b);
}

void *task(void *data) {
  Pack *p = (Pack *)data;
  foo(p->a, p->b);
  return NULL;
}

#define num_thread 5

void driver(int *a, int *b) {
  Pack p[num_thread];
  pthread_t t[num_thread];

  for (int i = 0; i < num_thread; ++ i) {
    p[i].a = a + i;
    p[i].b = b + i;
    pthread_create(t+i, NULL, task, p+i);
  }
  for (int i = 0; i < num_thread; ++ i) {
    pthread_join(t[i], NULL);
  } 
}

int main() {
  int a[num_thread] = { 1, 2, 3, 4, 5};
  int b[num_thread] = { 2, 3, 4, 5, 6};
  driver(a, b);
  return 0;
}