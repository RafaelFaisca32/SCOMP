#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "./consts.h"
#include "common.h"

int main() {
        void* mmapres;
        void* write;
        int   number[NUMBER], smfd, i;

        /* Create shared memory */
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        for (i = 0; i < NUMBER; i++) { number[i] = rand() % 21; }

        /* Write data to shared memory */
        printf("Writing data...\n");

        write = mmapres;
        smWriteBuff(&write, number, sizeof(number));

        /* Close shared memory */
        smClose(BUFF_SIZE, &mmapres, &smfd);

        printf("DONE\n");


        return 0;
}
