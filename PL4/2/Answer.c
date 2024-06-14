#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"

/* Semaphore to block writing to the output file */
#define SEM_NAME_W "/pl4ex2.w"
/* Base name for semaphores */
#define SEM_NAME_B "/pl4ex2.s."
/* Size for base name buffer */
#define BNAME_SIZE 25
/* Input file's name */
#define IN_FILE "Numbers.txt"
/* Output file's name */
#define OUT_FILE "Output.txt"
/* Number of child processes */
#define CHILD_N 8
/* Number of numbers to be read/writen by each child */
#define NUMBER_N 200
/* Macro to open a file or display a error message */
#define FOPEN_OR_ERROR(VAR, FILE, MODE, MESSAGE)                                                   \
        if ((VAR = fopen(FILE, MODE)) == NULL) {                                                   \
                fprintf(stderr, MESSAGE, strerror(errno));                                         \
                exit(EXIT_FAILURE);                                                                \
        }

int main() {
        /* Used to store the pid of each child */
        pid_t p[CHILD_N];
        /* The input and output files */
        FILE* file;
        /* Used to print the output file */
        char c;
        /* Iterator variables */
        int i, k;
        /* Base name for semaphores */
        char bname[BNAME_SIZE];

        /* Code for generating the input file */
        /* Open and clear input file */
        FOPEN_OR_ERROR(file, IN_FILE, "w", "Could not open input file \"" IN_FILE "\": [%s]\n");
        /* Put data on input file */
        for (i = 0; i < NUMBER_N * CHILD_N; i++) {
                for (k = 0; k < NUMBER_N; k++) {
                        if (fprintf(file, "%d\n", i * 10000 + k) < 0) {
                                fprintf(stderr, "Could not write to input file \"" IN_FILE "\"\n");
                                exit(EXIT_FAILURE);
                        }
                }
        }
        /* Close input file */
        fflush(file);
        fclose(file);

        /* Clearing output file */
        /* Open and clear output file */
        FOPEN_OR_ERROR(file, OUT_FILE, "w", "Could not open output file \"" OUT_FILE "\": [%s]\n");
        /* Close output file */
        fclose(file);

        /* Create children */
        for (i = 0; i < CHILD_N; i++) {
                /* Fork */
                p[i] = fork();
                if (p[i] == -1) {
                        /* Error */
                        perror("Could not fork.\n");
                        /* Free semaphores */
                        if (i != 0) {
                                /* If the semaphores were created, destroy them */
                                semFree(SEM_NAME_W);
                                for (k = 0; k <= i + 1; i++) {
                                        sprintf(bname, SEM_NAME_B "%d", k);
                                        semFree(bname);
                                }
                        }
                        exit(EXIT_FAILURE);
                } else if (p[i] == 0) {
                        /* Child */
                        /* Semaphore to block writing to output file */
                        sem_t* semw;
                        /* Semaphore for this child and the next */
                        sem_t *semCur, *semNext;
                        /* Iterator variable */
                        int j;
                        /* List of numbers read from input file */
                        int readNumbers[NUMBER_N];

                        /* Open semaphore */
                        semw = semOpen(SEM_NAME_W, 1);

                        /* Open Specific semaphore for this child and the next*/
                        sprintf(bname, SEM_NAME_B "%d", i);
                        semCur = semOpen(bname, i == 0);
                        sprintf(bname, SEM_NAME_B "%d", i + 1);
                        semNext = semOpen(bname, 0);

                        /* Request access to input resources */
                        printf("CHILD %d WAITING TO READ...\n", i);
                        semDown(semCur);
                        printf("CHILD %d READING...\n", i);


                        /* Open input file */
                        FOPEN_OR_ERROR(file, IN_FILE, "r",
                                       "Could not open input file \"" IN_FILE "\": [%s]\n");

                        /* Read numbers from input file */
                        for (k = 0; k < (i + 1) * NUMBER_N; k++) {
                                /* Read data from input file, check for reading error */
                                if (fscanf(file, "%d\n", &j) != 1) {
                                        fprintf(stderr,
                                                "Could not read from input file \"" IN_FILE "\"\n");
                                        /* Free semaphores */
                                        if (i != 0) {
                                                /* If the semaphores were created, destroy them */
                                                semFree(SEM_NAME_W);
                                                for (k = 0; k <= i + 1; k++) {
                                                        sprintf(bname, SEM_NAME_B "%d", k);
                                                        semFree(bname);
                                                }
                                        }
                                        fclose(file);
                                        exit(EXIT_FAILURE);
                                }
                                /* Read only the numbers belonging to this child */
                                if (k >= i * NUMBER_N) { readNumbers[k - (i * NUMBER_N)] = j; }
                        }

                        /* Free input resources */
                        fclose(file);
                        semUp(semNext);

                        /* Request access to output resources */
                        printf("CHILD %d WAITING TO WRITE...\n", i);
                        semDown(semw);
                        printf("CHILD %d WRITING...\n", i);

                        /* Open output file */
                        FOPEN_OR_ERROR(file, OUT_FILE, "a",
                                       "Could not open output file \"" OUT_FILE "\": [%s]\n");

                        /* Read numbers from input into ouput */
                        for (k = 0; k < NUMBER_N; k++) {
                                /* Write data to output file, check for writing error */
                                if (fprintf(file, "FROM CHILD NO [%d]: %d\n", i, readNumbers[k]) <
                                    0) {
                                        fprintf(stderr, "Could not write to output file \"" OUT_FILE
                                                        "\"\n");
                                        /* Free semaphores */
                                        if (i != 0) {
                                                /* If the semaphores were created, destroy them */
                                                semFree(SEM_NAME_W);
                                                for (k = 0; k <= i + 1; k++) {
                                                        sprintf(bname, SEM_NAME_B "%d", k);
                                                        semFree(bname);
                                                }
                                        }
                                        fclose(file);
                                        exit(EXIT_FAILURE);
                                }
                        }

                        /* Free resources */
                        fflush(file);
                        fclose(file);
                        semUp(semw);
                        semClose(&semw);
                        semClose(&semCur);
                        semClose(&semNext);
                        printf("CHILD %d DONE!\n", i);

                        /* Exit */
                        exit(EXIT_SUCCESS);
                }
        }

        /* Wait for children */
        for (i = 0; i < CHILD_N; i++) { waitpid(p[i], NULL, 0); }

        /* Open output file */
        FOPEN_OR_ERROR(file, OUT_FILE, "r", "Could not open output file \"" OUT_FILE "\": [%s]\n");

        /* Read everything in file */
        while ((c = fgetc(file)) != EOF) putchar(c);

        /* Close output file */
        fclose(file);

        /* Free semaphores */
        semFree(SEM_NAME_W);
        for (k = 0; k <= CHILD_N; k++) {
                sprintf(bname, SEM_NAME_B "%d", k);
                semFree(bname);
        }

        /* Exit */
        return EXIT_SUCCESS;
}
