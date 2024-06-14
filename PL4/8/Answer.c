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

#define SEM1_NAME "/pl4ex8.1"
#define SEM2_NAME "/pl4ex8.2"

int main() {
        pid_t  pid;
        sem_t *semPai, *semFilho;

        semPai   = semOpen(SEM1_NAME, 1); /* Open semaphore */
        semFilho = semOpen(SEM2_NAME, 1); /* Open semaphore */

        pid = fork();
        if (pid == -1) { /* verificação de erros nos processos */
                perror("Fork failed");
                return 1;
        } else if (pid == 0) { /* caso seja processo filho */
                while (1) {
                        semDown(semFilho);
                        printf("S\n");
                        semUp(semPai);
                        sleep(1);
                }
        }

        while (1) {
                semDown(semPai);
                printf("C\n");
                semUp(semFilho);
                sleep(1);
        }

        semFree(SEM1_NAME);
        semFree(SEM2_NAME);
        return 0;
}
