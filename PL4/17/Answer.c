#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "common.h"

/* Probability of child processes/spectators that are VIP */
#define CHLD_P_VIP 100
/* Probability of child processes/spectators that are SPECIAL */
#define CHLD_P_SPE 200
/* Probability of child processes/spectators that are NORMAL */
#define CHLD_P_NRM 300
/* Total number of children */
#define CHILD_N (CHLD_P_VIP + CHLD_P_SPE + CHLD_P_NRM)
/* Types of spectator */
#define TYPE_VIP 0
#define TYPE_SPE 1
#define TYPE_NRM 2
/* String descriptions of spectator */
#define DESC_VIP "  VIP  "
#define DESC_SPE "SPECIAL"
#define DESC_NRM " NORMAL"
/* Name of the semaphore used to sincronize the number of spectators in the room */
#define SEM_NAME_R "/pl4ex17.room"
/* Name of semaphore to allow another spectator into the room */
#define SEM_NAME_A "/pl4ex17.allow"
/* Name of semaphores used to allow the diffrent types of visitors in */
#define SEM_NAME_V "/pl4ex17.vip"
#define SEM_NAME_S "/pl4ex17.spe"
#define SEM_NAME_N "/pl4ex17.nrm"
/* Name of semaphore used to access shared memory area */
#define SEM_NAME_M "/pl4ex17.mutex"
/* Name of semaphore used to syncronyze when the processes begining */
#define SEM_NAME_I "/pl4ex17.init"
/* Maximum spectators in the room */
#define MAX_IN_ROOM 300
/* Max wait time for spectator stays in room */
#define MAX_ROOM_TIME 10
/* Name of shared memory area */
#define SHARED_MEMORY_NAME "/pl4ex17shm"
/* Size of shared memory area */
#define SHARED_MEMORY_SIZE sizeof(inline_t)

/* Contains information about who's waiting in line */
typedef struct {
        size_t vips;
        size_t specials;
        size_t normals;
        size_t totalVisitors;
} inline_t;

int main() {
        /* To store the PIDs of the child processes */
        pid_t p[CHILD_N];
        /* Iterators */
        size_t i, k;
        /* The semaphores */
        sem_t *semRoom, *semAllowNext, *semVip, *semSpe, *semNrm, *semMutex, *semInit;
        /* Who's waiting in line (stored in shared memory) */
        inline_t* qued;
        /* Shared memory file descriptor */
        int fd;
        /* Used to seed RNG */
        time_t t;

        /* Open and initialize shared memory */
        qued                = smOpenForWriting(SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE, &fd);
        qued->vips          = 0;
        qued->specials      = 0;
        qued->normals       = 0;
        qued->totalVisitors = 0;

        /* Create semaphores */
        semRoom      = semOpen(SEM_NAME_R, MAX_IN_ROOM);
        semAllowNext = semOpen(SEM_NAME_A, 0);
        semVip       = semOpen(SEM_NAME_V, 0);
        semSpe       = semOpen(SEM_NAME_S, 0);
        semNrm       = semOpen(SEM_NAME_N, 0);
        semMutex     = semOpen(SEM_NAME_M, 1);
        semInit      = semOpen(SEM_NAME_I, 0);

        /* Create childs */
        printf("STARTING SIMULATION...\n");
        fflush(stdout);
        /* Seed RNG */
        srand(time(&t));
        for (i = 0; i < CHILD_N; i++) {
                /* This child's type */
                int type;
                /* This childs textual description */
                char* desc;
                /* Wether the child was one of the first MAX_IN_ROOM visitors */
                int enteredFirst;
                /* Visitor no. */
                size_t visitorNumber;


                /* Set type of child*/
                type = rand() % CHILD_N;
                if (type < CHLD_P_VIP) {
                        type = TYPE_VIP;
                        desc = DESC_VIP;
                } else if (type < CHLD_P_VIP + CHLD_P_SPE) {
                        type = TYPE_SPE;
                        desc = DESC_SPE;
                } else {
                        type = TYPE_NRM;
                        desc = DESC_NRM;
                }

                /* Fork (create new process) */
                p[i] = fork();
                if (p[i] == -1) {
                        /* Fork error */
                        fprintf(stderr, "Could not fork.\n");
                        /* Free shared memory */
                        smFree(SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE, (void**)&qued, &fd);
                        /* Kill children and parent */
                        for (k = 0; k < i; k++) { kill(p[k], SIGKILL); }
                        for (k = 0; k < i; k++) { waitpid(p[k], NULL, 0); }
                        exit(EXIT_FAILURE);
                } else if (p[i] == 0) {
                        /* Seed RNG */
                        srand(time(&t) * i);

                        /* Open shared memory */
                        qued = smOpenForWriting(SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE, &fd);

                        /* Open semaphores */
                        semRoom      = semOpen(SEM_NAME_R, -1);
                        semAllowNext = semOpen(SEM_NAME_A, -1);
                        semVip       = semOpen(SEM_NAME_V, -1);
                        semSpe       = semOpen(SEM_NAME_S, -1);
                        semNrm       = semOpen(SEM_NAME_N, -1);
                        semMutex     = semOpen(SEM_NAME_M, -1);
                        semInit      = semOpen(SEM_NAME_I, -1);

                        /* Wait for all children to be created */
                        semDown(semInit);

                        /* Try to get in room */
                        enteredFirst = semTryDown(semRoom);
                        if (!enteredFirst) {
                                /* Could not get in room without waiting*/
                                /* Anounce to main process how many of each type are qued for entry
                                 */
                                semDown(semMutex);
                                switch (type) {
                                        case TYPE_VIP: qued->vips++; break;
                                        case TYPE_SPE: qued->specials++; break;
                                        case TYPE_NRM: qued->normals++; break;
                                }
                                semUp(semMutex);

                                /* Wait in the apropriate line */
                                switch (type) {
                                        case TYPE_VIP: semDown(semVip); break;
                                        case TYPE_SPE: semDown(semSpe); break;
                                        case TYPE_NRM: semDown(semNrm); break;
                                }
                        }

                        /* Keep tally of how many visitors room had */
                        semDown(semMutex);
                        visitorNumber = ++qued->totalVisitors;
                        semUp(semMutex);

                        /* Child is in room */
                        printf("[%.3ld]\t CHILD [%.3ld (%s)] GOT IN ROOM [%s WAIT].\n",
                               visitorNumber, i, desc, (enteredFirst) ? "DID NOT" : "HAD TO");
                        fflush(stdout);

                        /* Spend time in room */
                        sleep(1 + rand() % MAX_ROOM_TIME);

                        /* Child leaves room */
                        printf("[%.3ld]\t CHILD [%.3ld (%s)] LEFT THE ROOM.\n", visitorNumber, i,
                               desc);
                        fflush(stdout);

                        /* Allow next room visitor */
                        semUp(semAllowNext);

                        /* Close Shared memory */
                        smClose(SHARED_MEMORY_SIZE, (void**)&qued, &fd);

                        /* Close semaphore */
                        semClose(&semRoom);
                        semClose(&semAllowNext);
                        semClose(&semVip);
                        semClose(&semSpe);
                        semClose(&semNrm);
                        semClose(&semMutex);
                        semClose(&semInit);

                        /* Exit */
                        exit(0);
                }
        };

        /* All children were spawned, allow them to continue */
        printf("ALLOWING CHILDREN TO PROCEED...\n");
        fflush(stdout);
        for (i = 0; i < CHILD_N; i++) semUp(semInit);

        /* Start letting in visitors by priority */
        for (;;) {
                /* Wait for someone to leave room */
                semDown(semAllowNext);
                /* Wait for access to shared memory */
                semDown(semMutex);
                /* Allow a spectator to enter */
                if (qued->vips > 0) {
                        /* Allow a vip spectator in */
                        qued->vips--;
                        semUp(semVip);
                } else if (qued->specials > 0) {
                        /* Allow a special spectator in */
                        qued->specials--;
                        semUp(semSpe);
                } else if (qued->normals > 0) {
                        /* Allow a normal spectator in */
                        qued->normals--;
                        semUp(semNrm);
                } else {
                        /* No spectators were qued */
                        if (qued->totalVisitors == CHILD_N) {
                                /* All visitors have visited room */
                                semUp(semMutex);
                                break;
                        } else {
                                /* Release room */
                                semUp(semAllowNext);
                        }
                }

                /* Release mutex */
                semUp(semMutex);
        }

        /* Stop child processes */
        printf("WAITING FOR CHILDREN...\n");
        fflush(stdout);
        for (i = 0; i < CHILD_N; i++) { waitpid(p[i], NULL, 0); }

        /* Close semaphores */
        semClose(&semRoom);
        semClose(&semAllowNext);
        semClose(&semVip);
        semClose(&semSpe);
        semClose(&semNrm);
        semClose(&semMutex);
        semClose(&semInit);

        /* Free semaphores */
        semFree(SEM_NAME_R);
        semFree(SEM_NAME_A);
        semFree(SEM_NAME_V);
        semFree(SEM_NAME_S);
        semFree(SEM_NAME_N);
        semFree(SEM_NAME_M);
        semFree(SEM_NAME_I);

        /* Free shared memory */
        smFree(SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE, (void**)&qued, &fd);

        /* Exit */
        printf("SIMULATION DONE\n");
        fflush(stdout);

        return EXIT_SUCCESS;
}
