#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* The credit the child has in the begining of the game */
#define STARTING_CREDIT 20
/* Minimum number there can be in a bet */
#define MIN_RDM 1
/* Maximum number there can be in a bet */
#define MAX_RDM 5
/* Signifies that the child has credit */
#define HAS_CREDIT 1
/* Signifies that the child does not have credit */
#define NO_CREDIT 0
/* Credit gained by the child on a successful bet */
#define CREDIT_GAIN 10
/* Credit lost by the child on a unsuccessful bet */
#define CREDIT_LOSS 5

/* Helper function to generate random numbers */
int getRandomN() { return rand() % (MAX_RDM - MIN_RDM) + MIN_RDM; }

int main() {
        /* Used to open a pipe where the parent is the producer and the cild is the consumer */
        int fdp[2];
        /* Used to open a pipe where the cild is the producer and the parent is the consumer */
        int fdc[2];
        /* To store the pid of the child */
        pid_t p;
        /* To seed the RNG */
        time_t t;

        /* Attempt to open pipe */
        if (pipe(fdp) == -1 || pipe(fdc) == -1) {
                /* Error */
                perror("Could not open pipe.");
                exit(1);
        }

        /* Attempt to fork */
        p = fork();
        if (p == -1) {
                /* Fork error */
                perror("Could not fork.");
                exit(1);
        } else if (p == 0) {
                /* Child process */
                /* Close non used end of pipes */
                close(fdp[1]);
                close(fdc[0]);
                /* Seed RNG */
                srand(time(&t) + getpid());
                /* Main loop */
                for (;;) {
                        /* Child process */
                        /* Read wether to continue or to terminate */
                        int creditSignal = HAS_CREDIT;
                        read(fdp[0], &creditSignal, sizeof(int));
                        /* Continue betting only if there is still credit */
                        if (creditSignal == HAS_CREDIT) {
                                int myCredit;
                                /* Make bet */
                                int bet = getRandomN();
                                /* Tell parent the bet */
                                write(fdc[1], &bet, sizeof(int));
                                /* Read new credit */
                                read(fdp[0], &myCredit, sizeof(int));
                                /* Display credit */
                                printf("CREDIT: %d€\n", myCredit);
                        } else {
                                /* No credit, close pipes */
                                close(fdp[0]);
                                close(fdc[1]);
                                /* Terminate */
                                exit(0);
                        }
                }
        } else {
                /* Parent process */
                int childCredit = STARTING_CREDIT;
                /* Close non used end of pipes */
                close(fdp[0]);
                close(fdc[1]);
                /* Seed RNG */
                srand(time(&t) + getpid());
                /* Main loop */
                for (;;) {
                        /* Tell child wether to continue or end */
                        int creditSignal;
                        creditSignal = (childCredit > 0) ? HAS_CREDIT : NO_CREDIT;
                        write(fdp[1], &creditSignal, sizeof(int));
                        /* Continue reading bets only if child still has credit */
                        if (creditSignal == HAS_CREDIT) {
                                /* Read bet */
                                int bet;
                                read(fdc[0], &bet, sizeof(int));
                                /* Verify bet outcome */
                                if (bet == getRandomN()) {
                                        /* Increase credit */
                                        childCredit += CREDIT_GAIN;
                                } else {
                                        /* Decrease credit */
                                        childCredit -= CREDIT_LOSS;
                                }
                                /* Tell child it's new credit */
                                write(fdp[1], &childCredit, sizeof(int));
                        } else {
                                /* Child has no credit */
                                /* Close pipes */
                                close(fdp[1]);
                                close(fdc[0]);
                                /* Wait for child */
                                waitpid(p, NULL, 0);
                                /* Exit */
                                exit(0);
                        }
                }
        }

        return 0;
}
