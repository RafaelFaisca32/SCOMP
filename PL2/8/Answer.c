#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* The number of child processes that will be "competing" */
#define CHLD_N 10
/* The message to copy into the structure */
#define MESSAGE "Win"
/* The structure to pass trough the pipe */
typedef struct {
        int  round;
        char message[sizeof(MESSAGE)];
} winStructure;


int main() {
        /* Iterator and file descriptors */
        int i, fd[2];
        /* Pids of the child processes */
        pid_t pids[CHLD_N];

        /* Open pipes */
        if (pipe(fd) == -1) {
                perror("Could not open pipe.");
                exit(1);
        }

        /* Create child processes */
        for (i = 0; i < CHLD_N; i++) {
                /* Create child process */
                pids[i] = fork();
                if (pids[i] == -1) {
                        /* Fork error */
                        perror("Could not fork.");
                        exit(1);
                } else if (pids[i] == 0) {
                        winStructure st;
                        /* Close writing end of the pipe */
                        close(fd[1]);
                        /* Read structure from pipe */
                        read(fd[0], &st, sizeof(winStructure));
                        /* Close writing end of the pipe */
                        close(fd[0]);
                        /* Exit */
                        exit(st.round);
                }
        }

        /* Close reading end of the pipe */
        close(fd[0]);
        /* Write all rounds in in the pipe */
        for (i = 0; i < CHLD_N; i++) {
                /* Set-up struct */
                winStructure st;
                strcpy(st.message, MESSAGE);
                st.round = i + 1;
                /* Write struct to pipe */
                write(fd[1], &st, sizeof(winStructure));
        }
        /* Close writing end of pipe */
        close(fd[1]);

        /* Print winners */
        for (i = 0; i < CHLD_N; i++) {
                /* Get exit status of child */
                int stats;
                waitpid(pids[i], &stats, 0);
                /* Output the winners */
                printf("Winner of round %d is %ld\n", WEXITSTATUS(stats), (long)pids[i]);
        }

        return 0;
}
