#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "./consts.h"
#include "common.h"

int main() {
        int   smfd;
        void* mmapres;
        void* read;
        int   number[NUMBER], i;
        float average, sum = 0;

        /*Open shared memory */
        mmapres = smOpenForReading(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* Read data from shared memory */
        printf("Reading data...\n");

        read = mmapres;
        for (i = 0; i < NUMBER; i++) {
                number[i] = smReadInt(&read);
                printf("number : %d\n", number[i]);
                sum = sum + number[i];
        }

        average = sum / NUMBER;

        printf("Average is : %f\n", average);

        /* Close shared memory area */
        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);

        printf("DONE\n");

        return 0;
}
