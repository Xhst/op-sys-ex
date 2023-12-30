/*
Esercizio 4

Si scriva il codice di una funzione C con la seguente interfaccia void tunnel(int descriptors[], int count) tale che,
se eseguita, porti l’applicazione a gestire, per ogni file-descriptor dell’array descriptors l’inoltro del flusso dei 
dati in ingresso verso il canale di standard-output dell’applicazione. 
Il parametro count indica di quanti elementi è costituito l’array descriptors. 
L’inoltro dovrà essere attuato in modo concorrente per i diversi canali.

*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 100

void tunnel(int descriptors[], int count) {
    int r;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < count; i++) {
        while((r = read(descriptors[i], buffer, BUFFER_SIZE)) > 0) {
            printf("%s", buffer);
        }
        printf("\n");
    }
}

int main() {
    int descriptors[3];

    descriptors[0] = open("testo1.txt", O_RDONLY);
    descriptors[1] = open("testo2.txt", O_RDONLY);
    descriptors[2] = open("testo3.txt", O_RDONLY);

    tunnel(descriptors, 3);


}