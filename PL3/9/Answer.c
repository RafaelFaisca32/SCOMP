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

#define SHARED_MEMORY_NAME "/pl3ex9"
#define BUFF_SIZE          CHILD_N * sizeof(int)
#define CHILD_N            10
#define EACH_CHILD_FINDS   100
#define ARRAY_SIZE         (CHILD_N * EACH_CHILD_FINDS)

int main() {
        int * write, *read;
        void* mmapres;
        pid_t pid[CHILD_N];
        int   i, j, smfd, number[ARRAY_SIZE], max = 0, max1 = 0;

        for (i = 0; i < ARRAY_SIZE; i++) { /* gerar números aleatórios para o array */
                number[i] = rand() % 1001;
        }

        /* open à leitura e escrita na shared memory*/
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        for (i = 0; i < CHILD_N; i++) {
                pid[i] = fork(); /* fork 10 vezes devido ao for */

                if (pid[i] == -1) { /* verificação de erros nos processos */
                        perror("Fork failed");
                        return 1;
                } else if (pid[i] == 0) { /* caso seja processo filho */

                        /* open à leitura e escrita na shared memory */
                        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

                        for (j = i * EACH_CHILD_FINDS; j < (i + 1) * (EACH_CHILD_FINDS);
                             j++) { /* ciclo a passar pelos 100 elementos de cada processo filho */
                                if (max < number[j]) { max = number[j]; }
                        }
                        write    = mmapres;
                        write[i] = max; /* a escrever o valor maximo de cada filho na memoria
                                           partilhada */

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
                if (max1 < read[i]) { /* leitura da memória partilhada e comparação com o atual
                                         valor máximo */
                        max1 = read[i];
                }
        }

        printf("Biggest global number is %d\n", max1);

        /* Close shared memory area */
        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);
        return 0;
}
