#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "common.h"

#define SHARED_MEMORY_NAME "/pl3ex5"
#define TIMES              1000000
#define BUFF_SIZE          (TIMES * 2 * sizeof(int))

int main() {
        pid_t pid;
        void* mmapres;
        int*  read1;
        int   i, smfd, number1 = 8000, number2 = 200;

        pid = fork();
        if (pid == -1) {
                perror("Fork failed");
                return 1;
        }

        /* father */
        if (pid > 0) {
                /* Create shared memory */
                mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

                /* Read data from shared memory */
                printf("Reading data from shared memory on the father...\n");

                read1 = mmapres;
                for (i = 0; i < TIMES; i += 2) {
                        read1[i]     = number1;
                        read1[i + 1] = number2;
                }

                for (i = 0; i < TIMES; i += 2) {
                        read1[i]++;
                        read1[i + 1]--;
                }

                /* Close shared memory */
                smClose(BUFF_SIZE, &mmapres, &smfd);
        }

        /* child */
        else {
                /* Create shared memory */
                mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

                /* Read data from shared memory */
                printf("Reading data from shared memory on the child...\n");

                read1 = mmapres;
                for (i = 0; i < TIMES; i += 2) {
                        read1[i]     = number1;
                        read1[i + 1] = number2;
                }

                for (i = 0; i < TIMES; i += 2) {
                        read1[i]--;
                        read1[i + 1]++;
                }

                /* Close shared memory area */
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);
        }

        printf("Number 1: %d Number 2: %d\n", number1, number2);
        return 0;
}
