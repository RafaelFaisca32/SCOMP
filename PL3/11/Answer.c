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

#define SHARED_MEMORY_NAME "/pl3ex11"
#define BUFF_SIZE          CHILD_N * sizeof(int)
#define CHILD_N            10
#define EACH_CHILD_FINDS   100
#define ARRAY_SIZE         (CHILD_N * EACH_CHILD_FINDS)

int main() {
        pid_t pid[CHILD_N];
        int   i, j, fd[2], number[ARRAY_SIZE], max, max1;

        if (pipe(fd) == -1) { /* verificação de erros no pipe e a sua criação */
                perror("Pipe failed");
                return 1;
        }

        for (i = 0; i < ARRAY_SIZE; i++) { /* gerar números aleatórios para o array */
                number[i] = rand() % 1001;
        }

        for (i = 0; i < CHILD_N; i++) {
                pid[i] = fork(); /* fork 10 vezes devido ao for */

                if (pid[i] == -1) { /* verificação de erros nos processos */
                        perror("Fork failed");
                        return 1;
                } else if (pid[i] == 0) { /* caso seja processo filho */

                        close(fd[0]); /* close à leitura do pipe */
                        max = number[0];

                        for (j = i * EACH_CHILD_FINDS; j < (i + 1) * (EACH_CHILD_FINDS);
                             j++) { /* ciclo a passar pelos 100 elementos de cada processo filho */
                                if (max < number[j]) { max = number[j]; }
                        }
                        write(fd[1], &max, sizeof(max)); /* escreve no pipe o valor do máximo de
                                                            cada processo filho*/

                        close(fd[1]); /* close à escrita no pipe */
                        exit(EXIT_SUCCESS);
                }
        }

        for (i = 0; i < CHILD_N; i++) {
                waitpid(pid[i], NULL, 0); /* espera para que todos os processos filhos terminem */
        }

        close(fd[1]); /* close à escrita no pipe */

        for (i = 0; i < CHILD_N;
             i++) { /* for para verificar qual o máximo entre os máximos de todos os filhos */
                if (max1 < read(fd[0], &max1, sizeof(max1))) { /* leitura do pipe e comparação com o
                                                                  atual valor máximo */
                        max1 = read(fd[0], &max1, sizeof(max1));
                }
        }

        close(fd[0]); /* close à leitura no pipe */

        printf("Biggest global number is %d\n", max1);
        return 0;
}
