#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"

/* Base name for semaphores */
#define SEM_NAME "/pl4ex7.s."
/* Size for base name buffer */
#define BNAME_SIZE 25
/* Number of child processes */
#define CHLD_N 3
/* Number of steps */
#define STEP_N 2

int main() {
        /* Used to store the pids of the childs */
        pid_t p[CHLD_N];
        /* Counters */
        int i, j;
        /* Base name for semaphores */
        char bname[BNAME_SIZE];
        /* The semaphore for the current and next processes */
        sem_t *semCur, *semNext;

        for (i = 0; i < CHLD_N; i++) {
                p[i] = fork();
                if (p[i] == -1) {
                        /* Error */
                        perror("Could not fork.\n");
                        for (j = 0; j < i; j++) {
                                kill(p[j], SIGKILL);
                                waitpid(p[j], NULL, 0);
                        }
                        exit(EXIT_FAILURE);
                } else if (p[i] == 0) {
                        /* Open semaphore for this process an the next one */
                        sprintf(bname, SEM_NAME "%d", i);
                        semCur = semOpen(bname, i == 0);
                        sprintf(bname, SEM_NAME "%d", (i + 1) % CHLD_N);
                        semNext = semOpen(bname, ((i + 1) % CHLD_N) == 0);

                        /* For each step */
                        for (j = 0; j < STEP_N; j++) {
                                /* Request access */
                                semDown(semCur);

                                /* Do step according to step number and child*/
                                switch (j) {
                                        case 0:
                                                switch (i) {
                                                        case 0: printf("Sistemas "); break;
                                                        case 1: printf("de "); break;
                                                        case 2: printf("Computadores - "); break;
                                                        default: exit(EXIT_FAILURE);
                                                }
                                                fflush(stdout);
                                                break;
                                        case 1:
                                                switch (i) {
                                                        case 0: printf("a "); break;
                                                        case 1: printf("melhor "); break;
                                                        case 2: printf("disciplina! \n"); break;
                                                        default: exit(EXIT_FAILURE);
                                                }
                                                fflush(stdout);
                                                break;
                                        default: exit(EXIT_FAILURE);
                                }

                                /* Give acess */
                                semUp(semNext);
                        }

                        /* Close semaphores */
                        semClose(&semCur);
                        semClose(&semNext);

                        exit(EXIT_SUCCESS);
                }
        }

        /* Wait for children */
        for (i = 0; i < CHLD_N; i++) waitpid(p[i], NULL, 0);

        /* Free semaphores */
        for (i = 0; i < CHLD_N; i++) {
                sprintf(bname, SEM_NAME "%d", i);
                semFree(bname);
        }

        /* Exit */
        return EXIT_SUCCESS;
}
