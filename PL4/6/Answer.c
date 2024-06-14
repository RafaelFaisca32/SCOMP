#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"

/* Semaphore to block parent process */
#define SEM_NAME_P "/pl4ex6.p"
/* Semaphore to block child process */
#define SEM_NAME_C "/pl4ex6.c"
/* Number of messages */
#define MSG_N 15

int main() {
        /* Used to store the pid of the child */
        pid_t p;
        /* Used to block the this and the other process*/
        sem_t *semThis, *semOther;
        /* Counter */
        int i;

        p = fork();
        if (p == -1) {
                /* Error */
                perror("Could not fork.\n");
                exit(EXIT_FAILURE);
        } else {
                /* Open semaphore */
                if (p == 0) {
                        semThis  = semOpen(SEM_NAME_C, 1);
                        semOther = semOpen(SEM_NAME_P, 0);
                } else {
                        semThis  = semOpen(SEM_NAME_P, 0);
                        semOther = semOpen(SEM_NAME_C, 1);
                }

                for (i = 0; i < MSG_N; i += 2) {
                        /* Request to continue */
                        semDown(semThis);

                        /* Display and exit */
                        if (p == 0) printf("[%d] I’m the child\n", i);
                        else
                                printf("[%d] I’m the father\n", i + 1);

                        /* Allow to continue */
                        semUp(semOther);
                }

                /* Close semaphores */
                semClose(&semThis);
                semClose(&semOther);

                if (p != 0) {
                        /* Parent Waits for child */
                        waitpid(p, NULL, 0);

                        /* Free semaphores */
                        semFree(SEM_NAME_C);
                        semFree(SEM_NAME_P);
                }
        }

        /* Exit */
        return EXIT_SUCCESS;
}
