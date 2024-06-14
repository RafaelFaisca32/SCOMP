#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Size of lookup array */
#define ARRAY_SIZE 10000
/* Max number in array */
#define MAX_NUM 10000

int main() {
        /* To iterate */
        int i;
        /* The number to find */
        int lookUp;
        /* Lookup array */
        int numbers[ARRAY_SIZE];
        /* Nedded to seed the RNG */
        time_t t;
        /* Holder for child's pid*/
        pid_t p;

        /* Intializes RNG */
        srand((unsigned)time(&t));

        /* Initialize array with random numbers*/
        for (i = 0; i < ARRAY_SIZE; i++) numbers[i] = rand() % MAX_NUM;

        /* Initialize lookUp */
        lookUp = rand() % MAX_NUM;

        p = fork();
        if (p == -1) {
                perror("Could not fork.");
                exit(1);
        } else if (p == 0) {
                /* Child */
                /* Occurences found */
                int found = 0;
                /* Count occurences */
                for (i = 0; i < ARRAY_SIZE / 2; i++) {
                        if (numbers[i] == lookUp) { found++; }
                }
                /* Return value found */
                exit(found);
        } else {
                /* Parent */
                /* Status of child */
                int status;
                /* Occurences found */
                int found = 0;
                /* Count occurences */
                for (i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++) {
                        if (numbers[i] == lookUp) { found++; }
                }
                /* Wait for child */
                waitpid(p, &status, 0);
                /* Increment status */
                found += WEXITSTATUS(status);
                printf("Occurences of %d are %u\n", lookUp, found);
        }
}
