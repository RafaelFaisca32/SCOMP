#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"

/* Semaphore to block parent process */
#define SEM_NAME "/pl4ex5"

int main() {
        /* Used to store the pid of the child */
        pid_t p;
        /* Used to block the this and the other process*/
        sem_t* sem;

        p = fork();
        if (p == -1) {
                /* Error */
                perror("Could not fork.\n");
                exit(EXIT_FAILURE);
        } else {
                /* Open semaphore */
                sem = semOpen(SEM_NAME, 0);

                if (p == 0) {
                        /* Display */
                        printf("I’m the child\n");

                        /* Allow to continue */
                        semUp(sem);
                } else {
                        /* Request to continue */
                        semDown(sem);

                        /* Display */
                        printf("I’m the father\n");
                }

                /* Close semaphore */
                semClose(&sem);

                if (p != 0) {
                        /* Parent Waits for child */
                        waitpid(p, NULL, 0);

                        /* Free semaphore */
                        semFree(SEM_NAME);
                }
        }

        /* Exit */
        return EXIT_SUCCESS;
}
