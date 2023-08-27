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

#define num_thread 2

int main() {
  Pack p[num_thread];
  pthread_t t[num_thread];
  for (int i = 0; i < num_thread; ++ i) {
    pthread_create(t+i, NULL, task, p+i);
  }
  for (int i = 0; i < num_thread; ++ i) {
    pthread_join(t[i], NULL);
  } 
  // int a[num_thread];
  // int b[num_thread];
  // std::thread threads[num_thread];
  // for (int i = 0; i < num_thread; ++ i) {
  //   threads[i] = std::thread(foo, a + i, b + i);
  // }
  // for (int i = 0; i < num_thread; ++ i) {
  //   threads[i].join();
  // }
  return 0;
}