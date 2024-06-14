#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "common.h"
#include "consts.h"

int main() {
        int   smfd;
        void* mmapres;
        void* read;
        int   number;
        char  name[BUFF_SIZE - sizeof(int)];

        /*Open shared memory */
        mmapres = smOpenForReading(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* Read data from shared memory */
        printf("Reading data...\n");

        read   = mmapres;
        number = smReadInt(&read);
        smReadString(&read, name);

        printf("Number of the student: %d\nName of the student: %s\n", number, name);

        /* Close shared memory area */
        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);

        printf("DONE\n");

        return 0;
}
