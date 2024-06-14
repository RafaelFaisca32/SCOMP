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
        void* mmapres;
        void* write;
        int   number, fd;
        char  name[BUFF_SIZE - sizeof(int)];

        /* Create shared memory */
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &fd);

        /* Read input from user */
        do {
                setbuf(stdin, NULL);
                printf("PLEASE ENTER THE NUMBER OF THE STUDENT > ");
        } while (scanf("%d", &number) != 1);

        do {
                setbuf(stdin, NULL);
                printf("PLEASE ENTER THE NAME OF THE STUDENT > ");
        } while (fgets(name, sizeof(name), stdin) != name);


        /* Write data to shared memory */
        printf("Writing data...\n");

        write = mmapres;
        smWriteInt(&write, number);
        smWriteString(&write, name);

        /* Close shared memory */
        smClose(BUFF_SIZE, &mmapres, &fd);

        printf("DONE\n");

        return 0;
}
