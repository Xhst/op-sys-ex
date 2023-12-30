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
#include <unistd.h>
#include <pthread.h>

#include "parameters.h"

#define abort(msg) do{printf(msg);exit(1);}while(0)

short min,max;
void *shared_seg;

int find_max(int* array, int start, int end) {
    int max = array[start];

    for (int i = start + 1; i < end; i++) {
        if (array[i] > max) max = array[i];
    }

    return max;
}

typedef struct s_min_max {
    int min;
    int max;
} min_max;

int find_min(int* array, int start, int end) {
    int min = array[start];

    for (int i = start + 1; i < end; i++) {
        if (array[i] < min) min = array[i];
    }

    return min;
}

int array[6];


int chunk = 2;

void* find_min_max(void* args) {
    int i = (int) args;

    min_max* min_max_value = malloc(sizeof(min_max));

    min_max_value->max = find_max(array, chunk * i, chunk * (i+1));
    min_max_value->min = find_min(array, chunk * i, chunk * (i+1));

    pthread_exit((void*) (min_max_value));
}

int main() {
    int res;

    array[0] = 10;
    array[1] = 1;
    array[2] = 5000;
    array[3] = 190;
    array[4] = 1210;
    array[5] = 2;
    
    int min = 99999999999;
    int max = -1;

    pthread_t tids[3];

    for (int i = 0; i < 3; i++) {
        printf("Starting thread %d\n", i);
        pthread_create(tids+i, NULL, find_min_max, i);
    }

    for (int i = 0; i < 3; i++) {
        void* ret;
        pthread_join(tids[i], &ret);

        min_max* min_max_value = (min_max*) ret;

        if (min_max_value->max > max) max = min_max_value->max;
        if (min_max_value->min < min) min = min_max_value->min;


        printf("Per il thread %d Il minimo: %d, il massimo: %d \n", i, min_max_value->min, min_max_value->max);
    }

    printf("Il minimo: %d, il massimo: %d \n", min, max);

    return 0;
}


