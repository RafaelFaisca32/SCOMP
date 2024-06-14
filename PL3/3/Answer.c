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

#define SHARED_MEMORY_NAME "/pl3ex3"
#define BUFF_SIZE          100000

struct Ex3 {
        int   number;
        char* text;
};

int main() {
        int        i, fd[2], fd1[2], smfd;
        pid_t      pid;
        void*      mmapres;
        void*      write1;
        void*      read1;
        struct Ex3 arr[BUFF_SIZE];

        for (i = 0; i < BUFF_SIZE; i++) {
                arr[i].number = i;
                arr[i].text   = "ISEP – SCOMP 2020";
        }

        if (pipe(fd) == -1) {
                perror("Pipe failed");
                return 1;
        }

        if (pipe(fd1) == -1) {
                perror("Pipe failed");
                return 1;
        }

        /* Create shared memory */
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, sizeof(struct Ex3) * BUFF_SIZE, &smfd);

        pid = fork();
        if (pid == -1) {
                perror("Fork failed");
                return 1;
        }

        /* father */
        if (pid > 0) {
                clock_t start, stop, start1, stop1;
                int     ativ;
                close(fd[1]); /* close à escrita do pipe */
                start = clock();
                read(fd[0], &arr, BUFF_SIZE * sizeof(struct Ex3)); /* leitura do que foi escrita
                                                                      pelo processo filho */
                stop = clock() - start;
                close(fd[0]); /* close à leitura do pipe */

                printf("Time wasted reading on pipe : %ld\n", stop);

                /*Open shared memory */
                mmapres =
                        smOpenForReading(SHARED_MEMORY_NAME, sizeof(struct Ex3) * BUFF_SIZE, &smfd);

                /* Read data from shared memory */
                printf("Reading data from shared memory...\n");
                close(fd[1]);
                read(fd1[0], &ativ, sizeof(ativ));
                read1  = mmapres;
                start1 = clock();
                smReadBuff(&read1, arr, sizeof(arr));
                stop1 = clock() - start1;
                close(fd1[0]);

                /* Close shared memory */
                smClose(BUFF_SIZE, &mmapres, &smfd);

                printf("Time wasted reading on shared memory : %ld\n", stop1);
        }

        /* child */
        else {
                clock_t start, stop, start1, stop1;
                int     ativ = 1;
                close(fd[0]); /* close à leitura do pipe */
                close(fd1[0]);
                start = clock();
                write(fd[1], &arr, BUFF_SIZE * sizeof(struct Ex3)); /* escrita no pipe para eventual
                                                                       leitura pelo pai */
                stop = clock() - start;
                close(fd[1]);

                printf("Time wasted writing on pipe : %ld\n", stop);

                write1 = mmapres;
                start1 = clock();
                smWriteBuff(&write1, arr, sizeof(arr));
                stop1 = clock() - start1;
                write(fd1[1], &ativ, sizeof(ativ));
                close(fd1[1]);

                printf("Time wasted writing on shared memory : %ld\n", stop1);

                /* Close shared memory area */
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);

                exit(0);
        }
        return 0;
}
