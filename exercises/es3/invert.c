/*
Esercizio 3

Scrivere un programma C invert che dato un file A ne inverte il contenuto e lo memorizza in nuovo file B.
Il programma deve:

- riportare il contenuto di A in memoria;
- invertire la posizione di ciascun byte utilizzando un numero N di thread/processi concorrenti;
- scrivere il risultato in un nuovo file B.

A, B e N sono parametri che il programma deve acquisire da linea di comando.

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

struct invert_arguments {
    int thread;
    int chunk_size;
    int offset;
    char* buffer;
};

void* invert(void* arguments) {
    struct invert_arguments* args = (struct invert_arguments*) arguments;

    char* temp = malloc(sizeof(char) * args->chunk_size);

    for (int i = 0; i < args->chunk_size; i++) {
        temp[i] = args->buffer[args->offset + args->chunk_size - 1 - i];
    }

    pthread_exit((void*) temp);
}

int main() {

    char a[255];
    char b[255];
    int n;

    printf("Scrivere il nome del file da invertire: \n");
    scanf("%s", a);

    printf("Scrivere il nome dove salvare il nuovo contenuto invertito: \n");
    scanf("%s", b);

    printf("Specificare il numero di thread da utilizzare: \n");
    scanf("%d", &n);

    int file_a = open(a, O_RDONLY);

    if (file_a == -1) abort("Impossibile aprire il file.");

    // Puntatore a file alla fine per vedere la lunghezza del file.
    int file_size = lseek(file_a, 0, SEEK_END);

    // Rimetto il puntatore del file all'inizio.
    lseek(file_a, 0, SEEK_SET);

    // Il numero di thread non può essere maggiore della grandezza del file.
    n = n > file_size ? file_size : n;

    char* a_content = malloc(file_size);

    int r = read(file_a, a_content, file_size);

    if (r == -1) abort("Impossibile leggere il file.");

    int remaining_size = file_size;
    int chunk_size = file_size / n;
    
    pthread_t tids[n];
    struct invert_arguments args[n];

    for (int i = 0; i < n; i++) {
        remaining_size -= chunk_size;

        args[i].thread = i;

        // un chunk per ogni thread, il thread finale riceve anche la parte in piu' se presente 
        args[i].chunk_size = (i == (n - 1)) ? chunk_size + remaining_size : chunk_size;
        
        args[i].offset = i * chunk_size;
        args[i].buffer = a_content;

        pthread_create(tids+i, NULL, invert, (void*) (args+i));
    }
    
    char* result[n];

    for (int i = 0; i < n; i++) {
        void* ret;
        pthread_join(tids[i], &ret);

        result[i] = ret;
    }

    int file_b = open(b, O_CREAT | O_TRUNC | O_WRONLY, 660);

    if (file_b == -1) abort("Impossibile creare il file");

    for (int i = n - 1; i >= 0; i--) {
        write(file_b, result[i], args[i].chunk_size);
        free(result[i]);
    }

    free(a_content);
    close(file_a);
    close(file_b);
}