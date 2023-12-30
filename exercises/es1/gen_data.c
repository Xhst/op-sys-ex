/*
Esercizio 1

/dev/random -> (https://man7.org/linux/man-pages/man4/random.4.html)

Nei sistemi operitvi UNIX, /dev/urandom è un dispositivo a caratteri (char device) virtuale in grado di generare numeri casuali. 
Nello specifico, l’operazione di lettura dal relativo file produce byte casuali.
Scrivere un programma C che genera un file con contenuto interamente randomico. Il programma:
- prende come parametri da linea di comando: un numero N e una stringa S da usare come nome del file da creare;
- crea un file S contenente N byte randomici;
- utilizza il dispositivo /dev/random come sorgente di numeri pseudo-casuali.

 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include "parameters.h"

#define abort(msg) do{printf(msg);exit(1);}while(0)


int main(int argc, char *argv[]) {
    int n;
    char s[20]; 

    printf("Inserire il nome del file: \n");
    scanf("%s", s);

    printf("Inserisci il numero di byte che deve contenere il file: \n");
    scanf("%d", &n);

    int dev_rand = open("/dev/random", O_RDONLY);
    int file = open(s, O_TRUNC | O_CREAT | O_WRONLY, 660);

    if (dev_rand == -1) abort("Impossibile aprire /dev/random.\n");
    if (file == -1) abort("Impossibile creare il file.\n");

    size_t bytes_size = sizeof(short) * n; //sizeof(char)
    char* rand_bytes = malloc(bytes_size);

    read(dev_rand, rand_bytes, bytes_size);
    write(file, rand_bytes, bytes_size);

    close(dev_rand);
    close(file);
}