#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"

/* Beer semaphore */
#define SEM_NAME_B "/pl4ex9.beer"
/* Chips semaphore */
#define SEM_NAME_C "/pl4ex9.chips"

void buy_chips();
void buy_beer();
void eat_and_drink();

int main() {
        /* Used to store the pids of the childs */
        pid_t p;
        /* The semaphore for the beer and chips */
        sem_t *semBeer, *semChips;

        /* Fork */
        p = fork();
        if (p == -1) {
                /* Error */
                perror("Could not fork.\n");
                exit(EXIT_FAILURE);
        } else {
                /* Open semaphores */
                semBeer  = semOpen(SEM_NAME_B, 0);
                semChips = semOpen(SEM_NAME_C, 0);

                /* Buy (increment semaphore once for each child) */
                if (p == 0) {
                        buy_beer();
                        semUp(semBeer);
                        semUp(semBeer);
                } else {
                        buy_chips();
                        semUp(semChips);
                        semUp(semChips);
                }

                /* Try to eat and drink if beer and chips were bought */
                semDown(semBeer);
                semDown(semChips);
                eat_and_drink();

                /* Close semaphores */
                semClose(&semBeer);
                semClose(&semChips);

                /* Parent frees semaphores */
                if (p != 0) {
                        waitpid(p, NULL, 0);
                        semFree(SEM_NAME_B);
                        semFree(SEM_NAME_C);
                }
        }

        /* Exit */
        return EXIT_SUCCESS;
}

/* Functions to buy and eat and drink */
void buy_chips() {
        printf("BUGHT CHIPS\n");
        fflush(stdout);
}

void buy_beer() {
        printf("BOUGHT BEER\n");
        fflush(stdout);
}

void eat_and_drink() {
        printf("ATE AND DRANK\n");
        fflush(stdout);
}
