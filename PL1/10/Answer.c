#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


#define ARR_SIZE          2000 /* The size of the lookup array (preferabley a multiple of N_CHILDS) */
#define N_CHILDS          10   /* The number of child processes to create */
#define ERROR_RETURN      255                   /* Number to return when number is not found */
#define SEARCHED_BY_CHILD (ARR_SIZE / N_CHILDS) /* Numbers each child will search */
#define MAX_NUM           400                   /* Maximum number in array */

int main() {
        /* Needed to seed RNG */
        time_t t;
        /* To store child process's pids */
        pid_t pids[N_CHILDS];
        /* Iterator; number to find; and look up array */
        int i, lookUp, arr[ARR_SIZE];

        /* Fill array with random numbers */
        srand((unsigned)time(&t));
        for (i = 0; i < ARR_SIZE; i++) { arr[i] = rand() % MAX_NUM; }
        /* Random number to look up */
        lookUp = rand() % MAX_NUM;

        /* Create childs */
        for (i = 0; i < N_CHILDS; i++) {
                pid_t p = fork();
                if (p == -1) {
                        perror("Could not fork.");
                        exit(1);
                } else if (p > 0) {
                        /* Parent stores child process's pids */
                        pids[i] = p;
                } else {
                        /* Child tries to find lookUp in their partition of arr*/
                        int b;
                        for (b = 0; b < SEARCHED_BY_CHILD; b++) {
                                if (arr[(i * SEARCHED_BY_CHILD) + b] == lookUp) { exit(b); }
                        }
                        exit(ERROR_RETURN);
                }
        }

        /* Wait for children */
        for (i = 0; i < N_CHILDS; i++) {
                /* Status of child */
                int stat;
                /* Wait for child */
                waitpid(pids[i], &stat, 0);
                /* If the child found lookUp in their partition of arr... */
                if (WEXITSTATUS(stat) != ERROR_RETURN) {
                        /* Display the position of lookUp */
                        printf("Child %d foud number %d on relative address %d (absolute: %d)\n", i,
                               lookUp, WEXITSTATUS(stat),
                               (i * SEARCHED_BY_CHILD) + WEXITSTATUS(stat));
                }
        }

        return 0;
}
