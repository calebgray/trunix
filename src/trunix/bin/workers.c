#!/bin/cinit

#include <stdlib.h>
#include <pthread.h>

void *func() {
  return (int *) 2;
}

int main() {
  int i, numWorkers = 4, ret;
  pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * numWorkers);
  void *threadRet;
  for (int i = 0; i < numWorkers; ++i) {
    pthread_create(&threads[i], NULL, func, NULL);
  }
  for (int i = 0; i < numWorkers; ++i) {
    pthread_join(threads[i], &threadRet);
    ret += (int) threadRet;
  }
  return ret;
}
