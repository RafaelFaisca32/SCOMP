#define _XOPEN_SOURCE 700
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Size of random number array*/
#define ARR_SIZE 1000
/* Number of child processes to create */
#define N_CHILDS 5
/* How many nymbers are searced by each cild
idealy ARR_SIZE is divisable by N_CHILDS */
#define SEARCHED_BY_CHILD (ARR_SIZE / N_CHILDS)
/* Maximum random number in array */
#define MAX_NUM 255

int main() {
        /* To get random seed */
        time_t t;
        /* To store the pids of the child processes */
        pid_t p, pids[N_CHILDS];
        /* Maximum number found */
        int  max = INT_MIN;
        int  i, arr[ARR_SIZE], result[ARR_SIZE];
        int* aResult = result;

        /* Fill array with random numbers */
        srand((unsigned)time(&t));
        for (i = 0; i < ARR_SIZE; i++) { arr[i] = rand() % MAX_NUM; }

        /* Create childs */
        for (i = 0; i < N_CHILDS; i++) {
                p = fork();
                if (p == -1) {
                        perror("Could not fork.");
                        exit(1);
                } else if (p > 0) {
                        /* Parent stores child's pid */
                        pids[i] = p;
                } else {
                        /* Iterator */
                        int b;
                        /* Maximum local number found */
                        int max = INT_MIN;
                        /* Iterate trough the members that this child is
                        responsible for */
                        for (b = 0; b < SEARCHED_BY_CHILD; b++) {
                                /* Update max if needed */
                                if (arr[(i * SEARCHED_BY_CHILD) + b] > max) {
                                        max = arr[(i * SEARCHED_BY_CHILD) + b];
                                }
                        }
                        /* Exit by setting the exit value to max */
                        exit(max);
                }
        }

        /* Wait for children and find global max */
        for (i = 0; i < N_CHILDS; i++) {
                /* The exit status of the child */
                int stat;
                /* Wait for the child */
                waitpid(pids[i], &stat, 0);
                /* Update the global max if needed */
                if (WEXITSTATUS(stat) > max) { max = WEXITSTATUS(stat); }
        }

        p = fork();
        if (p == -1) {
                perror("Could not fork.");
                exit(1);
        } else if (p > 0) {
                /* parent */
                for (i = ARR_SIZE / 2; i < ARR_SIZE; i++) {
                        aResult[i] = (arr[i] / (float)max) * 100;
                }
                waitpid(p, NULL, 0);
                for (i = ARR_SIZE / 2; i < ARR_SIZE; i++) { printf("%d - %d\n", i, aResult[i]); }
        } else {
                /* child */
                for (i = 0; i < ARR_SIZE / 2; i++) {
                        aResult[i] = (arr[i] / (float)max) * 100;
                        printf("%d - %d\n", i, aResult[i]);
                }
                exit(0);
        }

        return 0;
}
