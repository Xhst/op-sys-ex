/*
Esercizio 5

Si scriva una funzione C con la seguente interfaccia void file_check(char *file_name, int num_threads).
Tale funzione dovrà lanciare num_thread nuovi threads, in modo che ciascuno di essi legga stringhe dallo standard input,
e per ogni stringa letta verifichi l’occorrenza di tale stringa all’interno di ciascuna riga del file il cui path 
è identificato tramite il parametro file_name, e stampi la stringa su standard output in caso affermativo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

struct cfw_args {
    int thread;
    int offset;
    int chunk_size;
    int file_descriptor;
};

// da prendere dallo standard input
char *words[5];

void* check_for_word(void* arguments) {
    struct cfw_args* args = (struct cfw_args*) arguments;

    char buffer[args->chunk_size];
    lseek(args->file_descriptor, args->offset, SEEK_SET);
    int r = readline(args->file_descriptor, buffer, args->chunk_size);

    for (int i = 0; i < 3; i++) {
        char* res = strstr(buffer, words[i]);
        if (!res) continue;

        printf("Thread %d has found the word: %s\n", args->thread, words[i]);
    }
}

void file_check(char *file_name, int num_threads) {
    pthread_t tids[num_threads];

    int file = open(file_name, O_RDONLY);
    if (file == -1) abort("Impossibile aprire il file");

    int file_size = lseek(file, 0, SEEK_END);
    lseek(file, 0, SEEK_SET);

    int remaining_size = file_size;
    int base_chunk_size = file_size / num_threads;

    struct cfw_args args[num_threads];

    for (int i = 0; i < num_threads; i++) {

        remaining_size -= base_chunk_size;

        args[i].thread = i;
        args[i].offset = i * base_chunk_size;
        args[i].chunk_size = (i == (num_threads - 1)) ? base_chunk_size + remaining_size : base_chunk_size;
        args[i].file_descriptor = file;

        pthread_create(tids+i, NULL, check_for_word, args+i);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(tids[i], NULL);
    }
}

int main() {
    words[0] = "hussam";
    words[1] = "saffy";
    words[2] = "juelma";
    words[3] = "badbsbe";
    words[4] = "noncista";

    file_check("text.txt", 4);
}

