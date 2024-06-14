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
        void *      mmapres, *read;
        int         selection, smfd, numberID, i, numberFOUND = 0;
        struct Ex10 users[RECORD_SIZE];
        sem_t*      sem1;

        sem1 = semOpen(SEM_NAME1, 1);

        /*Open à leitura da shared memory */
        mmapres = smOpenForReading(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        printf("1. Consult one user\n2. Consult multiple users\n");
        scanf("%d", &selection);

        switch (selection) {
                case 1:
                        printf("What is the identification number: ");
                        scanf("%d", &numberID);

                        read = mmapres;
                        for (i = 0; i < RECORD_SIZE; i++) {
                                semDown(sem1);

                                users[i].number = smReadInt(&read);
                                smReadString(&read, users[i].name);
                                smReadString(&read, users[i].address);

                                semUp(sem1);

                                if (numberID == users[i].number) {
                                        printf("Number: %d\t Name: %s\t Address: %s\n",
                                               users[i].number, users[i].name, users[i].address);
                                        numberFOUND = 1;
                                }
                        }

                        if (numberFOUND == 0) { printf("No user with that number\n"); }

                        break;

                case 2:
                        printf("All users: \n");

                        read = mmapres;
                        for (i = 0; i < RECORD_SIZE; i++) {
                                users[i].number = smReadInt(&read);
                                smReadString(&read, users[i].name);
                                smReadString(&read, users[i].address);

                                if (users[i].number > 0) {
                                        printf("Number: %d\t Name: %s\t Address: %s\n",
                                               users[i].number, users[i].name, users[i].address);
                                }
                        }
                        break;
        }

        /* Close shared memory area */
        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);

        return 0;
}
