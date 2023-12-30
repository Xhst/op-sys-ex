// si inseriscono da linea comandi tre parametri:
// 1) nome file di input
// 2) nome file output
// 3) numero dei thread

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

#define abort(msg) do{printf(msg); exit(1);} while(0)

int num_thread;
int file_lenght;
int slice_lenght;

char *input_buffer, *output_buffer;

int count;

void* function (void* param) {

	count++;
	int id = *(int*) param;
	
	printf("\nSono il thread numero: %d\n", count);

	for(int i=0; i<file_lenght; i++) {
		int idx_inizio=(slice_lenght*id)+i;
		int idx_fine=(file_lenght-1) - idx_inizio;
		output_buffer[idx_fine] = input_buffer[idx_inizio];

        printf("th: %d , i: %d, obif: %c\n", id, i, output_buffer[idx_fine]);
	}
	return NULL;
}

void inversione (int thread_count){

	count=0;
	int *indexes = malloc(thread_count*sizeof(pthread_t));
	pthread_t *tids = malloc(thread_count*sizeof(pthread_t));

	for(int i=0; i<thread_count; i++) {
		indexes[i]=i;
		pthread_create(tids+i, NULL, function, indexes+i);
	}

	for(int i=0; i<thread_count; i++)
		pthread_join(tids[i], NULL);

	free(tids);
  	free(indexes);
}

int main (int argc, char *argv[]) {

	if (argc != 4) abort("\nInserire tre parametri da linea comandi: file input, file output, #thread\n\n");
	char *in_file_path  = argv[1];       // parameter A
	char *out_file_path = argv[2];       // parameter B
	int num_threads     = atoi(argv[3]); // parameter N
	if(num_threads < 1) num_threads = 1;

	int ifd = open(in_file_path, O_RDONLY);
	if (ifd == -1) abort("\nErrore in apertura file input!\n\n");
	int ofd = open(out_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0660);
	if (ofd == -1) abort("\nErrore in creazione file output\n\n");

	size_t read_bytes   = 0;
  	size_t write_bytes  = 0;
	
	file_lenght = lseek(ifd, 0, SEEK_END);
	lseek(ifd, 0, SEEK_SET);
	slice_lenght=file_lenght/num_threads;

	input_buffer  = malloc(file_lenght+1);
  	output_buffer = malloc(file_lenght+1);
   
  	if(!input_buffer || !output_buffer) abort("Cannot allocate memory\n");

  	read_bytes = read(ifd, input_buffer, file_lenght);
   	if(read_bytes < 0) abort("Errore di lettura file di input!\n");

  	inversione(num_threads);

    write_bytes = write(ofd, output_buffer, file_lenght);
    if(write_bytes < 0) abort("Errore di scritura file di output!\n");
    
	printf("\nFine elaborazione, creati %d thread\n\n", count);

	close(ifd);
	close(ofd);

	// free allocated memory
  	free(input_buffer);
  	//free(output_buffer);
}