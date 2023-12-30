/*
Si scriva una funzione di codice C con la seguente interfaccia:
int above_threshold(char *filename, int n, int threshold).
Tale funzione deve lanciare un numero di thread/processi pari a n per leggere dal file binario
filename una sequenza di int e contare quanti di questi hanno un valore maggiore di
threshold. Infine, la funzione stampa su standard output e ritorna il numero totale di int con
valore maggiore di threshold contenuti nel file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

#define FILENAME "dataset.bin"
#define NUM_THREADS 4
#define NUM_THRESHOLDS 3

#define CHUNK_SIZE 512

int thresholds[NUM_THRESHOLDS] = {0x0, 0x40000000, 0xc0000000};

struct routine_params {
  int thread;
  int threshold;
  int file_descriptor;
};

void* routine(void* parameters) {
  struct routine_params* params = (struct routine_params*) parameters;

  size_t buffer_size = sizeof(int) * CHUNK_SIZE;
  int* buffer = malloc(buffer_size);
  int* countptr = malloc(sizeof(int));
  *countptr = 0;

  int r;

  while((r = read(params->file_descriptor, buffer, buffer_size)) > 0)
    for (int i = 0; i < r/sizeof(int); i++)
      if (buffer[i] > params->threshold) (*countptr)++;

  free(buffer);
  printf("Partial count of Thread %d: %d\n", params->thread, *countptr);

  pthread_exit((void*) (countptr));
}

int above_threshold(char *filename, int count, int threshold) {
  void* res;
  int total_count = 0;
  pthread_t* tids = malloc(sizeof(pthread_t) * count);
  struct routine_params* params = malloc(sizeof(struct routine_params) * count);

  int fd = open(filename, O_RDONLY);
  if (fd == -1) abort("Cannot open file");

  for (int i = 0; i < count; i++) {
    params[i].thread = i;
    params[i].threshold = threshold;
    params[i].file_descriptor = fd;
    
    pthread_create(tids+i, NULL, routine, params+i);
  }

  int* tcres[count];

  for (int i = 0; i < count; i++) {
    pthread_join(tids[i], &res);

    tcres[i] = (int*) res;
    total_count += *(tcres[i]);

    free(tcres[i]);
  }

  free(tids);
  free(params);

  return total_count;
}


int main(){
  int res;
  int i;

  for(i = 0; i < NUM_THRESHOLDS; i++) {
    res = above_threshold(FILENAME, NUM_THREADS, thresholds[i]);
    printf("The correct answer for %d is %d\n", thresholds[i], res);
  }
}

