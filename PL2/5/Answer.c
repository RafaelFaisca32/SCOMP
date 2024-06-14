#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main() {
        pid_t p;
        int   fdUP[2], fdDOWN[2];
        char  texto[256];

        if (pipe(fdUP) == -1) {
                perror("Pipe failed");
                return 1;
        }

        if (pipe(fdDOWN) == -1) {
                perror("Pipe failed");
                return 1;
        }

        p = fork();
        if (p == -1) {
                perror("Fork failed");
                return 1;
        }

        if (p > 0) { /* pai */
                int i, tamanho;
                close(fdDOWN[0]);                     /* close a leitura do pipe down */
                close(fdUP[1]);                       /* close a escrita do pipe up */
                tamanho = read(fdUP[0], &texto, 101); /* tamanho da frase lida do stdin */
                close(fdUP[0]);                       /* close a leitura do pipe up */
                for (i = 0; i < tamanho; i++) {
                        if (texto[i] >= 97 && texto[i] <= 122) { /* este if transforma as letras
                                                                    minusculas em maisculas */
                                texto[i] = texto[i] - 32;
                        } else if (texto[i] >= 65 &&
                                   texto[i] <= 90) { /* este if transforma as letras maisculas em
                                                        minusculas */
                                texto[i] = texto[i] + 32;
                        }
                }
                write(fdDOWN[1], &texto, tamanho); /* escreve no pipe down */
                close(fdDOWN[1]);                  /* close a escrita do pipe down */
        }

        if (p == 0) {             /* filho */
                close(fdUP[0]);   /* close a leitura do pipe up */
                close(fdDOWN[1]); /* close a escrita do pipe down */
                printf("Digite a mensagem, letras apenas: \n");
                scanf("%s", texto);
                write(fdUP[1], &texto,
                      strlen(texto)); /* escrita da mensagem no pipe up para a conversão dos
                                         caracteres feita pelo server (processo pai) */
                close(fdUP[1]);       /* close a escrita do pipe up */
                read(fdDOWN[0], &texto,
                     strlen(texto)); /* leitura da mensagem no pipe down que veio após a conversão
                                        de caracteres feita pelo server (processo pai) */
                close(fdDOWN[0]);    /* close a leitura do pipe down */
                printf("O cliente recebeu a mensagem: %s\n", texto);
        }

        return 0;
}
