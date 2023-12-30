#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "parameters.h"

void *process_func(void *param) {
    int* idptr = (int*)param; //Serve a differenziare i vari thread (va da 0 a #thread-1)
    int offset = (FILE_SIZE/CORES)*(id); //Calcola il punto di inizio

    short *local_data = malloc(MEM_PER_CORE); //Allocazione

    int ifd = open(FILENAME,O_RDONLY);

    lseek(ifd, offset, SEEK_CUR); //Riposiziona il file pointer

    int chunk_size = FILE_SIZE/CORES; //Dimensione del chunk
    int i = 0;
    int num_shorts = MEM_PER_CORE/sizeof(short);
    short min = 0x7fff, max = 0xffff;
    size_t size_r = 0;
    size_t r = 0;

    while(size_r != chunk_size) {
        r = read(ifd, local_data, MEM_PER_CORE);

        size_r += r;

        for(i=0; i<num_shorts; i++) {
            if(min > local_data[i]) min = local_data[i];
            if(max < local_data[i]) max = local_data[i];
        }
    }

    printf("I am %d with fd %d and offset %d, read bytes %ld, min %d, max %d\n", id, ifd, offset, size_r, min, max);

    return NULL;
}

int main() {
    int i = 0;
    pthread_t tids[CORES];
    int offset[CORES];
    void *res[CORES];

    for(i=0; i<CORES; i++) {
        offset[i] = i;
        pthread_create(tids+i, NULL, process_func, offset+i);
    }
    for(i=0; i<8; i++) {
        pthread_join(tids[i],res+i);
    }
}