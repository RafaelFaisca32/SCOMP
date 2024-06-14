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
        double sum = 0;
        int    smfd, i;
        void*  mmapres;
        char*  read;

        /* Open shared memory */
        mmapres = smOpenForReading(SHARED_MEMORY_NAME, CHAR_N + 1, &smfd);

        /* Read data from shared memory */
        read = mmapres;
        for (i = 0; i < CHAR_N; i++) { sum += read[i]; }
        printf("TEXT:    %s\nAVERAGE: %lf\n", read, sum / CHAR_N);

        /* Close shared memory area */
        smFree(SHARED_MEMORY_NAME, CHAR_N + 1, &mmapres, &smfd);

        return 0;
}
