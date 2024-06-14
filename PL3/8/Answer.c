#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common.h"

#define SHARED_MEMORY_NAME "/pl3ex8"
#define BUFF_SIZE          sizeof(int)
#define TIMES              1000000

int main() {
        int   smfd, i;
        void* mmapres;
        pid_t p;

        /* Create shared memory area */
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* Initial value */
        *smAs(int, mmapres) = 100;

        p = fork();
        if (p == -1) {
                /* Error */
                fprintf(stderr, "Could not fork.\n");
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);
                exit(EXIT_FAILURE);
        } else if (p == 0) {
                /* Child */
                /* Open shared memory for reading */
                mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);
        }

        /* Increment and decrement */
        for (i = 0; i < TIMES; i++) {
                *smAs(int, mmapres) += 1;
                *smAs(int, mmapres) -= 1;
        }


        /* Child exits, parent waits for child to exit */
        if (p == 0) {
                /* Close shared memory */
                smClose(BUFF_SIZE, &mmapres, &smfd);
                exit(EXIT_SUCCESS);
        } else {
                waitpid(p, NULL, 0);

                /* Display result */
                printf("RESULT: %d\n", *smAs(int, mmapres));

                /* Close shared memory area */
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);

                exit(EXIT_SUCCESS);
        }
        return 0;
}
