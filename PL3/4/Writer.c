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
#include "common.h"
#include "consts.h"

int main() {
        int    i, fd;
        void*  mmapres;
        char*  write;
        time_t t;

        /* Create shared memory */
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &fd);

        /* Generate random chars and write data to shared memory */
        srand((unsigned)time(&t));
        write = mmapres;
        for (i = 0; i < CHAR_N; i++) { write[i] = 'a' + (rand() % ('z' - 'a' + 1)); }
        write[CHAR_N] = '\0';

        /* Close shared memory */
        smClose(BUFF_SIZE, &mmapres, &fd);

        return 0;
}
