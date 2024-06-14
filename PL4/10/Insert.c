#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "common.h"
#include "consts.h"

int main() {
        void *      mmapres, *write;
        int         smfd;
        struct Ex10 users;
        sem_t*      sem1;

        sem1 = semOpen(SEM_NAME1, 1);

        /* open à leitura e escrita na shared memory*/
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        printf("Please enter your number > ");
        scanf("%d", &users.number);

        printf("Please enter your name > ");
        scanf("%s", users.name);

        printf("Please enter your address > ");
        scanf("%s", users.address);

        /* Write data to shared memory */
        printf("Writing data...\n");

        semDown(sem1);

        write = mmapres;
        smWriteInt(&write, users.number);
        smWriteString(&write, users.name);
        smWriteString(&write, users.address);

        semUp(sem1);

        /* Close shared memory */
        smClose(BUFF_SIZE, &mmapres, &smfd);

        printf("DONE\n");

        /* Free semaphore */
        semFree(SEM_NAME1);

        return 0;
}
