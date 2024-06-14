#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "./consts.h"
#include "common.h"

int main() {
        void*  mmapres;
        int    smfd;
        time_t timeNow;
        char*  timeNowString;
        sem_t* wmutex;

        /* Create shared memory */
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* Abertura do Semáforo */
        wmutex = semOpen(SEMW, 1);

        /* processo está na Writer */
        semDown(wmutex);

        smAs(struct Ex14, mmapres)->active_writers++;

        time(&timeNow);
        timeNowString = ctime(&timeNow);

        sprintf(smAs(struct Ex14, mmapres)->str, "The PID is %d and the time at the moment is %s\n",
                getpid(), timeNowString);

        printf("Pid of the Writer: %d\tCurrent time: %s\tWriters working at the moment: "
               "%d\tReaders working at the moment: %d\n",
               getpid(), timeNowString, smAs(struct Ex14, mmapres)->active_writers,
               smAs(struct Ex14, mmapres)->active_readers);

        sleep(3);

        /* processo no Writer terminou */
        semUp(wmutex);

        /* Close shared memory */
        smClose(BUFF_SIZE, &mmapres, &smfd);

        return 0;
}
