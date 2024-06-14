#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int spawn_childs(int n) {
        pid_t p;
        int   i;
        for (i = 0; i < n; i++) {
                p = fork();
                if (p == -1) {
                        perror("Fork failed");
                        return -1;
                }
                if (p == 0) { /*caso seja filho*/
                        return i + 1;
                }
        }
        return 0; /*caso seja pai*/
}

int main() {
        int i, fd[6][2], id_crianca; /* para ligar os 6 processos entre sí são precisos 6 pipes */

        for (i = 0; i < 6; i++) {
                if (pipe(fd[i]) == -1) {
                        perror("Pipe failed");
                        return 1;
                }
        }

        id_crianca = spawn_childs(5); /* cria 5 processos filhos */

        if (id_crianca == 0) { /* pai */
                int    i, random, randomP;
                time_t t;        /* To get random seed */
                close(fd[0][0]); /* close à leitura do pipe que liga o pai à child1 */
                for (i = 0; i < 5; i++) {
                        if (i != 0) {
                                close(fd[i][0]); /* close a todos as leituras de pipes exceto as que
                                                    o if de cima retira */
                                close(fd[i][1]); /* close a todos as escritas de pipes exceto as que
                                                    o if de cima retira */
                        }
                }
                srand((unsigned)time(&t)); /* Get random time to wait */
                random = rand() % 500 + 1; /* random value */
                printf("Numero: %d e PID: %d\n", random, getpid());
                write(fd[0][1], &random, sizeof(random)); /* escrita do numero aleatorio no pipe que
                                                             liga o pai à child1 */
                close(fd[0][1]); /* close à escrita no pipe que liga o pai à child1 */
                read(fd[5][0], &randomP,
                     sizeof(randomP)); /* leitura do pipe que liga a child5 ao pai */
                printf("This is the biggest number: %d\n", randomP);
        }

        if (id_crianca > 0) { /* filho */
                int    random, randomP;
                time_t t;             /* To get random seed */
                switch (id_crianca) { /* dependendo do id da criança */
                        case 1:
                                close(fd[0]
                                        [1]); /* close a todos os pipes de leitura e escrita exceto
                                                 a leitura do pipe que liga o pai à child1 e a
                                                 escrita do pipe que liga a child1 à child2 */
                                close(fd[1][0]);
                                close(fd[2][0]);
                                close(fd[2][1]);
                                close(fd[3][0]);
                                close(fd[3][1]);
                                close(fd[4][0]);
                                close(fd[4][1]);
                                close(fd[5][0]);
                                close(fd[5][1]);
                                srand(time(&t) + getpid()); /* Get random time to wait */
                                random = rand() % 500 + 1;
                                printf("Numero: %d e PID: %d\n", random, getpid());
                                read(fd[0][0], &randomP,
                                     sizeof(randomP)); /* leitura do pipe que liga o pai à child1 */
                                if (random > randomP) {
                                        write(fd[1][1], &random,
                                              sizeof(int)); /* escrita no pipe que liga a child1 à
                                                               child2 */
                                } else {
                                        write(fd[1][1], &randomP,
                                              sizeof(int)); /* escrita no pipe que liga a child1 à
                                                               child2 */
                                }
                                close(fd[0]
                                        [0]); /* close à leitura do pipe que liga o pai à child1 */
                                close(fd[1][1]); /* close à escrita do pipe que liga a child1 à
                                                    child2 */
                                break;

                        case 2:
                                close(fd[1]
                                        [1]); /* close a todos os pipes de leitura e escrita exceto
                                                 a leitura do pipe que liga a child1 à child2 e a
                                                 escrita do pipe que liga a child2 à child3 */
                                close(fd[2][0]);
                                close(fd[0][0]);
                                close(fd[0][1]);
                                close(fd[3][0]);
                                close(fd[3][1]);
                                close(fd[4][0]);
                                close(fd[4][1]);
                                close(fd[5][0]);
                                close(fd[5][1]);
                                srand(time(&t) + getpid()); /* Get random time to wait */
                                random = rand() % 500 + 1;
                                printf("Numero: %d e PID: %d\n", random, getpid());
                                read(fd[1][0], &randomP,
                                     sizeof(randomP)); /* leitura do pipe que liga a child1 à child2
                                                        */
                                if (random > randomP) {
                                        write(fd[2][1], &random,
                                              sizeof(int)); /* escrita no pipe que liga a child2 à
                                                               child3 */
                                } else {
                                        write(fd[2][1], &randomP,
                                              sizeof(int)); /* escrita no pipe que liga a child2 à
                                                               child3 */
                                }
                                close(fd[1][0]); /* close à leitura do pipe que liga a child1 à
                                                    child2 */
                                close(fd[2][1]); /* close à escrita do pipe que liga a child2 à
                                                    child3 */
                                break;

                        case 3:
                                close(fd[2]
                                        [1]); /* close a todos os pipes de leitura e escrita exceto
                                                 a leitura do pipe que liga a child2 à child3 e a
                                                 escrita do pipe que liga a child3 à child4 */
                                close(fd[3][0]);
                                close(fd[0][1]);
                                close(fd[0][0]);
                                close(fd[1][1]);
                                close(fd[1][0]);
                                close(fd[4][1]);
                                close(fd[4][0]);
                                close(fd[5][1]);
                                close(fd[5][0]);
                                srand(time(&t) + getpid()); /* Get random time to wait */
                                random = rand() % 500 + 1;
                                printf("Numero: %d e PID: %d\n", random, getpid());
                                read(fd[2][0], &randomP,
                                     sizeof(randomP)); /* leitura do pipe que liga a child2 à child3
                                                        */
                                if (random > randomP) {
                                        write(fd[3][1], &random,
                                              sizeof(int)); /* escrita no pipe que liga a child3 à
                                                               child4 */
                                } else {
                                        write(fd[3][1], &randomP,
                                              sizeof(int)); /* escrita no pipe que liga a child3 à
                                                               child4 */
                                }
                                close(fd[2][0]); /* close à leitura do pipe que liga a child2 à
                                                    child3 */
                                close(fd[3][1]); /* close à escrita do pipe que liga a child3 à
                                                    child4 */
                                break;

                        case 4:
                                close(fd[3]
                                        [1]); /* close a todos os pipes de leitura e escrita exceto
                                                 a leitura do pipe que liga a child3 à child4 e a
                                                 escrita do pipe que liga a child4 à child5 */
                                close(fd[4][0]);
                                close(fd[0][1]);
                                close(fd[0][0]);
                                close(fd[1][1]);
                                close(fd[1][0]);
                                close(fd[2][1]);
                                close(fd[2][0]);
                                close(fd[5][1]);
                                close(fd[5][0]);
                                srand(time(&t) + getpid()); /* Get random time to wait */
                                random = rand() % 500 + 1;
                                printf("Numero: %d e PID: %d\n", random, getpid());
                                read(fd[3][0], &randomP,
                                     sizeof(randomP)); /* leitura do pipe que liga a child3 à child4
                                                        */
                                if (random > randomP) {
                                        write(fd[4][1], &random,
                                              sizeof(int)); /* escrita no pipe que liga a child4 à
                                                               child5 */
                                } else {
                                        write(fd[4][1], &randomP,
                                              sizeof(int)); /* escrita no pipe que liga a child4 à
                                                               child5 */
                                }
                                close(fd[3][0]); /* close à escrita do pipe que liga a child3 à
                                                    child4 */
                                close(fd[4][1]); /* close à escrita do pipe que liga a child4 à
                                                    child5 */
                                break;

                        case 5:
                                close(fd[4]
                                        [1]); /* close a todos os pipes de leitura e escrita exceto
                                                 a leitura do pipe que liga a child4 à child5 e a
                                                 escrita do pipe que liga a child5 ao pai */
                                close(fd[5][0]);
                                close(fd[0][0]);
                                close(fd[0][1]);
                                close(fd[1][0]);
                                close(fd[1][1]);
                                close(fd[2][0]);
                                close(fd[2][1]);
                                close(fd[3][0]);
                                close(fd[3][1]);
                                srand(time(&t) + getpid()); /* Get random time to wait */
                                random = rand() % 500 + 1;
                                printf("Numero: %d e PID: %d\n", random, getpid());
                                read(fd[4][0], &randomP,
                                     sizeof(randomP)); /* leitura do pipe que liga a child4 à child5
                                                        */
                                if (random > randomP) {
                                        write(fd[5][1], &random,
                                              sizeof(int)); /* escrita no pipe que liga a child5 ao
                                                               pai */
                                } else {
                                        write(fd[5][1], &randomP,
                                              sizeof(int)); /* escrita no pipe que liga a child5 ao
                                                               pai */
                                }
                                close(fd[4][0]); /* close à escrita do pipe que liga a child4 à
                                                    child5 */
                                close(fd[5]
                                        [1]); /* close à escrita do pipe que liga a child5 ao pai */
                                break;
                }
        }
}
