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
#include <time.h>
#include <unistd.h>
#include "common.h"

#define SHARED_MEMORY_NAME "/pl3ex6"
#define NUM_COUNT          1000000
#define NUM_TYPE           int
#define BUFF_SIZE          (sizeof(NUM_TYPE) * NUM_COUNT)
#define XSTR(X)            #X
#define STR(X)             XSTR(X)

/**
 * @brief Get the time difference between two times.
 * Adapted from
 * https://stackoverflow.com/questions/16275444/how-to-print-time-difference-in-accuracy-of-milliseconds-and-nanoseconds-from-c.
 *
 * @param start The start time.
 * @param end The end time.
 * @return double The time it took in seconds to go from the strat to the end.
 */
double getTimeDiff(struct timespec* start, struct timespec* end) {
        return ((double)end->tv_sec + 1.0e-9 * end->tv_nsec) -
               ((double)start->tv_sec + 1.0e-9 * start->tv_nsec);
}

int main() {
        pid_t    p;
        int      syncp[2], syncc[2], fd[2];
        int      tmp = 0;
        NUM_TYPE arr[NUM_COUNT];
        void*    shm;
        int      smfd;
        /* Iterator */
        size_t i;

        /* Open shared memory */
        shm = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* Open pipes */
        if (pipe(syncc) == -1 || pipe(syncp) == -1 || pipe(fd) == -1) {
                fprintf(stderr, "Could not pipe.\n");
                exit(EXIT_FAILURE);
        }

        /* Fork */
        p = fork();
        if (p == -1) {
                /* Fork error */
                fprintf(stderr, "Could not fork.\n");
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &shm, &smfd);
                exit(EXIT_FAILURE);
        } else if (p == 0) {
                /* Child */
                struct timespec start, end;

                /* Close unused pipe ends */
                close(fd[1]);
                close(syncp[1]);
                close(syncc[0]);

                /* Open memory for reading */
                shm = smOpenForReading(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

                /* Tell parent child is ready */
                write(syncc[1], &tmp, sizeof(int));

                /* Wait for parent to start writing */
                read(syncp[0], &tmp, sizeof(int));

                /* Read numbers from pipe */
                printf("READING NUMBERS FROM PIPE...\n");
                clock_gettime(CLOCK_MONOTONIC, &start);
                for (i = 0; i < NUM_COUNT; i++) { read(fd[0], arr + i, sizeof(NUM_TYPE)); }
                clock_gettime(CLOCK_MONOTONIC, &end);

                /* Close unused pipe ends */
                close(fd[0]);

                /* Send times to parent process */
                write(syncc[1], &start, sizeof(struct timespec));
                write(syncc[1], &end, sizeof(struct timespec));

                /* Wait for parent to start writing the data */
                read(syncp[0], &tmp, sizeof(int));

                /* Transfer numbers by shared memory */
                printf("READING NUMBERS FROM SHARED MEMORY...\n");
                clock_gettime(CLOCK_MONOTONIC, &start);
                for (i = 0; i < NUM_COUNT; i++) { arr[i] = smAs(NUM_TYPE, shm)[i]; }
                clock_gettime(CLOCK_MONOTONIC, &end);

                /* Send times to parent process */
                write(syncc[1], &start, sizeof(struct timespec));
                write(syncc[1], &end, sizeof(struct timespec));

                /* Close sync pipe */
                close(syncc[1]);
                close(syncp[0]);

                /* Close shared memory */
                smClose(BUFF_SIZE, &shm, &smfd);

                /* Exit */
                exit(EXIT_SUCCESS);

        } else {
                /* Parent */
                /* Times to send and receive data from pipes */
                struct timespec psstart;
                struct timespec psend;
                struct timespec prstart;
                struct timespec prend;
                /* Times to send and receive data from shared memory */
                struct timespec ssstart;
                struct timespec ssend;
                struct timespec srstart;
                struct timespec srend;

                /* Close unused pipe ends */
                close(fd[0]);
                close(syncc[1]);
                close(syncp[0]);

                /* Initialize array */
                for (i = 0; i < NUM_COUNT; i++) { arr[i] = i; }

                /* Wait for child to initialize */
                read(syncc[0], &tmp, sizeof(int));

                /* Tell child to proceed */
                write(syncp[1], &tmp, sizeof(int));

                /* Transfer numbers by pipe */
                printf("WRITING NUMBERS TO PIPE...\n");
                clock_gettime(CLOCK_MONOTONIC, &psstart);
                for (i = 0; i < NUM_COUNT; i++) { write(fd[1], arr + i, sizeof(NUM_TYPE)); }
                clock_gettime(CLOCK_MONOTONIC, &psend);

                /* Close unused pipe ends */
                close(fd[1]);

                /* Read time taken by child */
                read(syncc[0], &prstart, sizeof(struct timespec));
                read(syncc[0], &prend, sizeof(struct timespec));

                /* Tell child to proceed */
                write(syncp[1], &tmp, sizeof(int));

                /* Transfer numbers by shared memory */
                printf("WRITING NUMBERS TO SHARED MEMORY...\n");
                clock_gettime(CLOCK_MONOTONIC, &ssstart);
                for (i = 0; i < NUM_COUNT; i++) { smAs(NUM_TYPE, shm)[i] = arr[i]; }
                clock_gettime(CLOCK_MONOTONIC, &ssend);

                /* Read time taken by child */
                read(syncc[0], &srstart, sizeof(struct timespec));
                read(syncc[0], &srend, sizeof(struct timespec));

                /* Close sync pipe */
                close(syncp[1]);
                close(syncc[0]);

                /* Wait for child to finish */
                waitpid(p, NULL, 0);

                /* Free shared memory */
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &shm, &smfd);

                /* Display times */
                printf(
                        "-----------------------------------------------------------------------\n"
                        "TIMES FOR TYPE \"" STR(NUM_TYPE) "\":\n"
                        "        TO SEND DATA BY PIPE: ...................... %.5lfs\n"
                        "        TO RECEIVE DATA BY PIPE: ................... %.5lfs\n"
                        "        TO SEND DATA BY SHARED MEMORY: ............. %.5lfs\n"
                        "        TO RECEIVE DATA BY SHARED MEMORY: .......... %.5lfs\n"
                        "SPEED GAIN BY USING SHARED MEMORY (FOR SENDING): ... %.5lfs (%+2.2f%%)\n"
                        "SPEED GAIN BY USING SHARED MEMORY (FOR RECEIVING): . %.5lfs (%+2.2f%%)\n",
                        getTimeDiff(&psstart, &psend), /* TO SEND DATA BY PIPE */
                        getTimeDiff(&prstart, &prend), /* TO RECEIVE DATA BY PIPE */
                        getTimeDiff(&ssstart, &ssend), /* TO SEND DATA BY SHARED MEMORY */
                        getTimeDiff(&srstart, &srend), /* TO RECEIVE DATA BY SHARED MEMORY */
                        /* SPEED GAIN BY USING SHARED MEMORY (FOR SENDING) */
                        getTimeDiff(&psstart, &psend) - getTimeDiff(&ssstart, &ssend),
                        /* Percentage */
                        (getTimeDiff(&ssstart, &ssend)-getTimeDiff(&psstart, &psend))/getTimeDiff(&psstart, &psend)*100,
                        /* SPEED GAIN BY USING SHARED MEMORY (FOR RECEIVING) */
                        getTimeDiff(&prstart, &prend) - getTimeDiff(&srstart, &srend),
                        /* Percentage */
                        (getTimeDiff(&srstart, &srend) - getTimeDiff(&prstart, &prend))/getTimeDiff(&prstart, &prend)*100);

                /* Exit */
                exit(EXIT_SUCCESS);
        }

        return 0;
}
