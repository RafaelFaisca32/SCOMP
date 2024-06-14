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

#define SHARED_MEMORY_NAME "/pl4ex3"
#define BUFF_SIZE          STRING_N* CHAR_N
#define STRING_N           50
#define CHAR_N             80
#define CHILD_N            4

int main() {
        void * mmapres, *write;
        int    i, smfd;
        char   pidtxt[CHAR_N];
        pid_t  pid[CHILD_N];
        sem_t* sem;

        /* open à leitura e escrita na shared memory*/
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        for (i = 0; i < CHILD_N; i++) {
                pid[i] = fork();
                if (pid[i] == -1) { /* verificação de erros nos processos */
                        perror("Fork failed");
                        return 1;
                } else if (pid[i] == 0) { /* caso seja processo filho */

                        /* open do semáforo */
                        sem = semOpen(SHARED_MEMORY_NAME, 1);

                        /* decrementa o semáforo */
                        semDown(sem);

                        /* open à leitura e escrita na shared memory*/
                        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

                        /* passa o getppid para string e junta ao resto da string */
                        sprintf(pidtxt, "I’m the Father - with  PID %d\n", getpid());

                        write = mmapres;

                        /* write aponta para a posição do primeiro char a ser escrito por este filho
                         * na memória partilhada */
                        write = smAs(char, write) + CHAR_N * i;
                        smWriteString(&write, pidtxt);

                        /* Close shared memory area */
                        smClose(BUFF_SIZE, &mmapres, &smfd);

                        sleep(1);

                        /* Incrementa o semáforo */
                        semUp(sem);
                        exit(0);
                }
        }

        for (i = 0; i < CHILD_N; i++) {
                waitpid(pid[i], NULL, 0); /* espera para que todos os processos filhos terminem */
        }

        write = mmapres;

        for (i = 0; i < CHILD_N; i++) { printf("%s", smAs(char, write) + CHAR_N * i); }

        /* Close shared memory area */
        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);

        /* Free semaphore */
        semFree(SHARED_MEMORY_NAME);
        return 0;
}
