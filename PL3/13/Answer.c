#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "common.h"

struct Ex13 {
        char* path;
        char* wordsearch;
        int   noccur;
};

#define SHARED_MEMORY_NAME "/pl3ex13"
#define BUFF_SIZE          CHILD_N * sizeof(struct Ex13)
#define CHILD_N            10
#define FILE_STRING        300

int main() {
        int *       write, *read;
        void*       mmapres;
        pid_t       pid[CHILD_N];
        int         i, smfd, contador = 0;
        struct Ex13 arr[CHILD_N];

        for (i = 0; i < CHILD_N; i++) { /* gerar valores para a struct */
                arr[i].path       = "example.txt";
                arr[i].wordsearch = "ola";
        }

        /* para o filho 1 e o filho 2 serem diferentes dos restantes para mostrar a funcionalidade
         * do codigo */
        arr[0].wordsearch = "xau";
        arr[1].wordsearch = "bom";

        /* open à leitura e escrita na shared memory*/
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        for (i = 0; i < CHILD_N; i++) {
                pid[i] = fork(); /* fork 10 vezes devido ao for */

                if (pid[i] == -1) { /* verificação de erros nos processos */
                        perror("Fork failed");
                        return 1;
                } else if (pid[i] == 0) { /* caso seja processo filho */
                        char  str[FILE_STRING];
                        FILE* file;

                        /* open à leitura e escrita na shared memory */
                        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

                        /* Open file for reading */
                        file = fopen(arr[i].path, "r");
                        if (file == NULL) {
                                perror("File could not be opened for reading");
                                exit(1);
                        }

                        /* Passar linha à linha */
                        while (fgets(str, BUFF_SIZE, file) != NULL) {
                                char* position = str;
                                while ((position = (strstr(position, arr[i].wordsearch))) !=
                                       NULL) { /* This function takes two strings s1 and s2 as an
                                                  argument and finds the first occurrence of the
                                                  sub-string s2 in the string s1 */
                                        contador++;
                                        position++;
                                }
                        }

                        arr[i].noccur = contador;
                        write         = mmapres;
                        write[i] = arr[i].noccur; /* a escrever o valor do contador de cada filho na
                                                     memoria partilhada */

                        /* Close shared memory area */
                        smClose(BUFF_SIZE, &mmapres, &smfd);
                        exit(EXIT_SUCCESS);
                }
        }

        for (i = 0; i < CHILD_N; i++) {
                waitpid(pid[i], NULL, 0); /* espera para que todos os processos filhos terminem */
        }

        read = mmapres;
        for (i = 0; i < CHILD_N;
             i++) { /* for para verificar qual o máximo entre os máximos de todos os filhos */
                contador = read[i];
                printf("Numero de ocorrências na child %d foram %d\n", i + 1, contador);
        }

        /* Close shared memory area */
        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);
        return 0;
}
