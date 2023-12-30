/*
Esercizio 2

Dato un file binario contenente un sequenza di 2^15 interi di tipo short, scrivere un programma che crea N processi o threads,
i quali leggono il contenuto del file ed individuano il valore minimo e massimo contenuto nel file.
Nel fornire una soluzione rispettare i seguenti vincoli:

- ciascun intero non può essere letto da più di un thread/processo;

- ciascun thread/processo può leggere il medesimo intero al più una volta;

- ciascun thread/processo può allocare memoria nell’heap per al più 512 byte;

- N è un parametro definito a tempo di compilazione o tramite linea di comando;

- N è minore o uguale a 8;

- è ammesso allocare di variabili globali (data) e locali (stack) per memorizzare tipi primitivi 
  (puntatori, int, short, char, long, etc.) per al più 128 byte.

- Per generare il file è possibile utilizzare la soluzione dell’esercizio 1.
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>

#include "parameters.h"

typedef struct min_max {
    short min;
    short max;
} min_max_t;

min_max_t calc_min_max(short* array, int size) {
    min_max_t min_max;
    min_max.max = SHRT_MIN;
    min_max.min = SHRT_MAX;

    for (int i = 0; i < size; i++) {     
        min_max.min = array[i] < min_max.min ? array[i] : min_max.min;
        min_max.max = array[i] > min_max.max ? array[i] : min_max.max;
    }

    return min_max;
}

void* routine(void* args) {
    // file descriptor
    int fd = *(int*)args;

    int r;

    short* buffer = malloc(MEM_PER_CORE);
    min_max_t* local_min_max; = malloc(sizeof(min_max_t));
    local_min_max->max = SHRT_MIN;
    local_min_max->min = SHRT_MAX;

    min_max_t new_min_max;
    while((r = read(fd, buffer, MEM_PER_CORE)) > 0) {  
        new_min_max = calc_min_max(buffer, r/sizeof(short));

        if (new_min_max.min < local_min_max->min) local_min_max->min = new_min_max.min;
        if (new_min_max.max > local_min_max->max) local_min_max->max = new_min_max.max;
    }

    free(buffer);

    pthread_exit((void*) (local_min_max));
}

int main() {

    pthread_t tids[CORES];

    min_max_t total_min_max;
    total_min_max.max = SHRT_MIN;
    total_min_max.min = SHRT_MAX;

    min_max_t* threads_min_max[CORES];

    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1) abort("Impossibile aprire il file");

    for (int i = 0; i < CORES; i++) pthread_create(tids+i, NULL, routine, &fd);

    for (int i = 0; i < CORES; i++) {
        pthread_join(tids[i], (void*) &threads_min_max[i]);  

        // calcolo minimo e massimo totale con i minimi e massimi relativi dei vari thread.
        if (threads_min_max[i]->min < total_min_max.min) total_min_max.min = threads_min_max[i]->min;
        if (threads_min_max[i]->max > total_min_max.max) total_min_max.max = threads_min_max[i]->max;

        printf("Il thread %d ha ottenuto come minimo: %d; come massimo: %d\n", i, threads_min_max[i]->min, threads_min_max[i]->max);
        
        free(threads_min_max[i]);      
    }

    printf("Il minimo totale: %d; il massimo totale: %d\n", total_min_max.min, total_min_max.max);
    
    close(fd);

    return 0;
}


