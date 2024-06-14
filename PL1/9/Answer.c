#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Number of child processes to create */
#define N_CHILDS 10
/* How many numbers to display */
#define N_PER_CHILD 100

int main() {
        /* To iterate*/
        int i;
        /* To store pids */
        pid_t p, pids[N_CHILDS];

        /* Create each child process*/
        for (i = 0; i < N_CHILDS; i++) {
                p = fork();
                if (p == -1) {
                        perror("Could not fork.");
                        exit(1);
                } else if (p > 0) {
                        /* Parent stores child process's pids */
                        pids[i] = p;
                } else {
                        /* Iterator */
                        int b;
                        /* Child displays numbers*/
                        for (b = 0; b < N_PER_CHILD; b++) {
                                printf("%d\n", (i * N_PER_CHILD) + b + 1);
                        }
                        /* Kill child after it is done */
                        exit(0);
                }
        }

        /* Wait for each child to complete */
        for (i = 0; i < N_CHILDS; i++) { waitpid(pids[i], NULL, 0); }

        return 0;
}
