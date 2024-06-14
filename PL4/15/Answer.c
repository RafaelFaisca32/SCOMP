#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "common.h"

/* Name of the shared memory area used to tell how many children are ready for next show */
#define SHARED_MEMORY_NAME "/pl4ex15.ready"
/* Size of shared memory area */
#define SHARED_MEMORY_SIZE sizeof(size_t)
/* Name of the semaphore used to sincronize the number of visitors in the room */
#define SEM_NAME_R "/pl4ex15.room"
/* Name of the semaphore used at the begining of the show */
#define SEM_NAME_S "/pl4ex15.start"
/* Name of the semaphore used at the end of the show */
#define SEM_NAME_E "/pl4ex15.end"
/* Name of the semaphore used to sync how many visitors are ready for next show */
#define SEM_NAME_Y "/pl4ex15.sync"
/* Name of the semaphore used to start next show */
#define SEM_NAME_M "/pl4ex15.mutex"
/* Number of child processes/visitors */
#define CHLD_N 20
/* Number of shows to simulate */
#define SHOW_N 5
/* Maximum visitors in the room */
#define MAX_IN_ROOM 5
/* Duration of the show */
#define SHOW_DUR 3


int main() {
        /* To store the PIDs of the child processes */
        pid_t p[CHLD_N];
        /* Iterators */
        int i, k, fd;
        /* How many children are ready to start next show */
        size_t* waitingForStart;
        /* The semaphores */
        sem_t *semRoom, *semStart, *semEnd, *semNextShow, *semMutex;

        /* How many children are waiting for show to start */
        waitingForStart  = smOpenForWriting(SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE, &fd);
        *waitingForStart = 0;

        /* Open semaphores */
        semRoom     = semOpen(SEM_NAME_R, MAX_IN_ROOM);
        semStart    = semOpen(SEM_NAME_S, 0);
        semEnd      = semOpen(SEM_NAME_E, 0);
        semNextShow = semOpen(SEM_NAME_Y, 0);
        semMutex    = semOpen(SEM_NAME_M, 1);

        /* Create childs */
        printf("RUNNING SIMULATION FOR [%d] SHOWS OF [%d] SECCONDS...\n", SHOW_N, SHOW_DUR);
        fflush(stdout);
        for (k = 0; k < CHLD_N; k++) {
                /* Fork (create new process) */
                p[k] = fork();
                if (p[k] == -1) {
                        /* Fork error */
                        fprintf(stderr, "Could not fork.\n");
                        /* Free shared memory */
                        smFree(SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE, (void*)&waitingForStart,
                               &fd);
                        exit(EXIT_FAILURE);
                } else if (p[k] == 0) {
                        /* Client */
                        time_t t;

                        /* How many children are waiting for show to start */
                        waitingForStart =
                                smOpenForWriting(SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE, &fd);

                        /* Seed RNG */
                        srand(time(&t) * k);

                        /* Open semaphores */
                        semRoom     = semOpen(SEM_NAME_R, -1);
                        semStart    = semOpen(SEM_NAME_S, -1);
                        semEnd      = semOpen(SEM_NAME_E, -1);
                        semNextShow = semOpen(SEM_NAME_Y, -1);
                        semMutex    = semOpen(SEM_NAME_M, -1);

                        /* Main visitor simulation loop */
                        for (;;) {
                                /* Request to enter room */
                                semDown(semRoom);
                                printf("        VISITOR [%d] ENTERED ROOM\n", k);
                                fflush(stdout);
                                for (i = 1; i == 1;) {
                                        /* Tell parent how many visitors are waiting for the show to
                                         * start */
                                        semDown(semMutex);
                                        *waitingForStart += 1;
                                        if (*waitingForStart == MAX_IN_ROOM) {
                                                /* Allow next show when room is full */
                                                *waitingForStart = 0;
                                                semUp(semNextShow);
                                        }
                                        semUp(semMutex);
                                        /* Wait for show to start */
                                        semDown(semStart);
                                        printf("        VISITOR [%d] IS WATCHING THE SHOW...\n", k);
                                        /* Wait for show to end */
                                        semDown(semEnd);
                                        /* Decide to stay or leave the room at random */
                                        if (rand() % 2) {
                                                /* Leave the room */
                                                printf("        VISITOR [%d] LEFT ROOM\n", k);
                                                fflush(stdout);
                                                i = 0;
                                        } else {
                                                /* Stay in the room */
                                                printf("        VISITOR [%d] STAYED IN ROOM\n", k);
                                                fflush(stdout);
                                        }
                                }
                                /* Allow another visitor to enter the room */
                                semUp(semRoom);
                        }
                }
        }

        /* Simulate shows */
        for (i = 0; i < SHOW_N; i++) {
                /* Show begins */
                printf("[%d] SHOW BEGINING...\n", i);
                fflush(stdout);
                /* Let visitor know the show is starting */
                for (k = 0; k < MAX_IN_ROOM; k++) { semUp(semStart); }
                /* Wait for show to end */
                sleep(SHOW_DUR);
                /* Show ends */
                printf("[%d] SHOW ENDING...\n", i);
                fflush(stdout);
                /* Let visitors know the show ended */
                for (k = 0; k < MAX_IN_ROOM; k++) { semUp(semEnd); }
                semDown(semNextShow);
        }

        /* Stop child processes */
        printf("WAITING FOR CHILDREN...\n");
        fflush(stdout);
        for (i = 0; i < CHLD_N; i++) {
                kill(p[i], SIGKILL);
                waitpid(p[i], NULL, 0);
        }

        /* Close semaphore */
        semClose(&semRoom);
        semClose(&semStart);
        semClose(&semEnd);
        semClose(&semNextShow);
        semClose(&semMutex);

        /* Free semaphores */
        semFree(SEM_NAME_R);
        semFree(SEM_NAME_S);
        semFree(SEM_NAME_E);
        semFree(SEM_NAME_Y);
        semFree(SEM_NAME_M);

        /* Free shared memory */
        smFree(SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE, (void**)&waitingForStart, &fd);

        /* Exit */
        printf("SIMULATION DONE\n");
        fflush(stdout);

        return EXIT_SUCCESS;
}
