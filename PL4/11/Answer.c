#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "common.h"

/* Seat semaphores */
#define SEM_NAME_S "/pl4ex11.seats"
/* Mutex semaphore for shared memory */
#define SEM_NAME_X "/pl4ex11.mtex"
/* Mutex door semaphore base name */
#define SEM_NAME_B SEM_NAME_X "."
/* Train shared memory size */
#define SHARED_MEMORY_SIZE sizeof(train_t)
/* Size for base name buffer */
#define BNAME_SIZE 50
/* Number of doors */
#define DOOR_N 3
/* Max ocupancy */
#define MAX_OCUP 200
/* passenger number */
#define PASS_N 300

typedef struct {
        char inTrain;
} passenger_t;

typedef struct {
        passenger_t passengers[PASS_N];
} train_t;

int main() {
        /* To store the pids of the child processes */
        pid_t p[PASS_N];
        /* The semaphore the doors and the train ocupancy
        semOcupancy ensures that the  train never has less
        than 0 available seats */
        sem_t *semDoor, *semOcupancy, *semMutex;
        /* Iterators */
        int i, inTrain;
        /* For srand */
        time_t t;
        /* Base name for semaphores */
        char bname[BNAME_SIZE];
        /* Train info */
        train_t train;
        int     vacantSeats;

        /* Init train */
        memset(&train, 0, sizeof(train_t));
        vacantSeats = MAX_OCUP;
        for (i = 0; i < PASS_N & vacantSeats >= 1; i++) {
                /* Seed RNG */
                srand(time(&t) * i);

                /* Randomply decide to be in train or not */
                train.passengers[i].inTrain = rand() % 2;

                /* Update vacant seats */
                if (train.passengers[i].inTrain) vacantSeats--;
        }

        /* Create children */
        for (i = 0; i < PASS_N; i++) {
                /* Fork */
                p[i] = fork();
                if (p[i] == -1) {
                        /* Error */
                        perror("Could not fork.\n");
                        exit(EXIT_FAILURE);
                } else if (p[i] == 0) {
                        /* Child */
                        /* Door tring to use */
                        int door;

                        /* Open semaphores */
                        semOcupancy = semOpen(SEM_NAME_S, vacantSeats);
                        semMutex    = semOpen(SEM_NAME_X, 1);

                        /* Seed RNG */
                        srand(time(&t) * i);

                        /* Continue untill passenger has left train */
                        for (inTrain = 1; inTrain;) {
                                /* Select a random door to try to use */
                                door = rand() % DOOR_N;

                                /* Open door semaphore */
                                sprintf(bname, SEM_NAME_B "%d", door);
                                semDoor = semOpen(bname, 1);

                                /* Wait for door to be available */
                                printf("PASSENGER [%d] WAITING FOR DOOR %d\n", i, door);
                                fflush(stdout);
                                semDown(semDoor);

                                /* Wait to access shared memory */
                                semDown(semMutex);

                                /* Chose action */
                                if (train.passengers[i].inTrain) {
                                        /* passeger is trying to leave */
                                        train.passengers[i].inTrain = 0;
                                        inTrain                     = 0;
                                        semUp(semOcupancy);
                                        printf("PASSENGER [%d] LEFT THE TRAIN, %d SEATS "
                                               "AVAILABLE\n",
                                               i, semValue(semOcupancy));
                                } else {
                                        /* passeger is trying to enter */
                                        train.passengers[i].inTrain = 1;
                                        inTrain                     = 1;
                                        semDown(semOcupancy);
                                        printf("PASSENGER [%d] ENTERED THE TRAIN, %d SEATS "
                                               "AVAILABLE\n",
                                               i, semValue(semOcupancy));
                                }
                                fflush(stdout);

                                /* Release shared memory */
                                semUp(semMutex);

                                /* Allow others to use door */
                                semUp(semDoor);

                                /* Close door semaphore */
                                semClose(&semDoor);
                        }

                        /* Close semaphores */
                        semClose(&semOcupancy);
                        semClose(&semMutex);

                        exit(EXIT_SUCCESS);
                }
        }

        /* Wait for children */
        for (i = 0; i < PASS_N; i++) { waitpid(p[i], NULL, 0); }

        /* Free semaphores */
        for (i = 0; i < DOOR_N; i++) {
                sprintf(bname, SEM_NAME_B "%d", i);
                semFree(bname);
        }
        semFree(SEM_NAME_S);
        semFree(SEM_NAME_X);

        /* Exit */
        return EXIT_SUCCESS;
}
