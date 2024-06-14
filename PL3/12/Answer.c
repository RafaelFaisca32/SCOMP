#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
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

#define SHARED_MEMORY_NAME "/pl3ex12"
#define BUFF_SIZE          (sizeof(struct aluno) + sizeof(int))
#define STR_SIZE           50
#define NR_DISC            10

struct aluno {
        int  numero;
        char nome[STR_SIZE];
        int  disciplinas[NR_DISC];
};

int main() {
        /* To store PIDs */
        pid_t p;
        /* Shared memory location, has space for the "aluno" struct and an int for syncronization */
        struct aluno* shm;
        /* Shared memory file descriptor */
        int smfd;
        /* Iterator */
        size_t i;

        /* Open shared memory area */
        shm = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* Initialize sync variable */
        *smAs(int, smAs(char, shm) + sizeof(struct aluno)) = 0;

        /* Fork (create new process) */
        p = fork();
        if (p == -1) {
                /* Fork error */
                fprintf(stderr, "Could not fork.\n");
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, smAs(void*, &shm), &smfd);
                exit(EXIT_FAILURE);
        } else if (p == 0) {
                /* Child */
                int max, min, sum;

                /* Init values */
                max = INT_MIN;
                min = INT_MAX;
                sum = 0;

                /* Open memory for reading */
                shm = smOpenForReading(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

                /* Wait for parent to finish writing */
                while (*smAs(int, smAs(char, shm) + sizeof(struct aluno)) == 0)
                        ;

                /* Claculate max and min */
                for (i = 0; i < NR_DISC; i++) {
                        if (max < shm->disciplinas[i]) max = shm->disciplinas[i];
                        if (min > shm->disciplinas[i]) min = shm->disciplinas[i];
                        sum += shm->disciplinas[i];
                }

                /* Display data */
                printf("STUDENT:\n"
                       "        NUMBER: .... %d\n"
                       "        NAME: ...... %s\n"
                       "        MAX GRADE: . %d\n"
                       "        MIN GRADE: . %d\n"
                       "        AVG GRADE: . %.2f (MAS DEVIA SER 20)\n"
                       "        GRADES: .... ",
                       shm->numero, shm->nome, max, min, ((float)sum / NR_DISC));
                for (i = 0; i < NR_DISC; i++) { printf("%d  ", shm->disciplinas[i]); }
                printf("\n");

                /* Close shared memory */
                smClose(BUFF_SIZE, smAs(void*, &shm), &smfd);

                /* Exit */
                exit(EXIT_SUCCESS);
        } else {
                /* Parent */
                time_t t;

                /* Initialize student */
                shm->numero = 1192232;
                strcpy(shm->nome, "ANDRÉ RIBEIRO");
                srand(time(&t));
                for (i = 0; i < NR_DISC; i++) { shm->disciplinas[i] = (rand() % 11) + 10; }

                /* Tell child student was initialized */
                *smAs(int, smAs(char, shm) + sizeof(struct aluno)) = 1;

                /* Wait for child to finish */
                waitpid(p, NULL, 0);

                /* Close shared memory */
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, smAs(void*, &shm), &smfd);

                /* Exit */
                exit(EXIT_SUCCESS);
        }

        return 0;
}
