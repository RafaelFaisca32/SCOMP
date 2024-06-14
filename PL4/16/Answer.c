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

#define CHILD_N            6
#define SHARED_MEMORY_NAME "/pl4ex16"
#define BUFF_SIZE          sizeof(int) * 2
#define SEM_EAST           "/semEast"
#define SEM_WEST           "/semWest"

struct Ex16 {
        int westCars;
        int eastCars;
};


int main() {
        void*  mmapres;
        sem_t *east, *west;
        pid_t  pid[CHILD_N];
        int    smfd, i;

        /* Create shared memory */
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* valores iniciais */
        smAs(struct Ex16, mmapres)->westCars = 4;
        smAs(struct Ex16, mmapres)->eastCars = 2;

        east = semOpen(SEM_EAST, 0);
        west = semOpen(SEM_WEST, 1);

        for (i = 0; i < CHILD_N; i++) {
                pid[i] = fork(); /* fork 6 vezes devido ao for */

                if (pid[i] == -1) { /* verificação de erros nos processos */
                        perror("Fork failed");
                        return 1;

                } else if (pid[i] == 0) { /* caso seja processo filho */

                        if (smAs(struct Ex16, mmapres)->westCars > 0) {
                                /* Carro entrou na via oeste */
                                semDown(west);

                                while (smAs(struct Ex16, mmapres)->westCars != 0) {
                                        sleep(1);
                                        smAs(struct Ex16, mmapres)->westCars--;
                                        printf("One car from the west went through\n");
                                }

                                /* Carro saiu da via oeste logo permite a entrada de carros do este
                                 */
                                semUp(east);
                        }
                        if (smAs(struct Ex16, mmapres)->eastCars > 0) {
                                /* Carro entrou na via este */
                                semDown(east);

                                while (smAs(struct Ex16, mmapres)->eastCars != 0) {
                                        sleep(1);
                                        smAs(struct Ex16, mmapres)->eastCars--;
                                        printf("One car from the east went through\n");
                                }

                                /* Carro saiu da via este logo permite a entrada de carros do
                                 * oeste*/
                                semUp(west);
                        }
                }
        }

        /* caso seja pai */

        smClose(BUFF_SIZE, &mmapres, &smfd);

        sem_unlink(SEM_EAST);
        sem_unlink(SEM_WEST);

        return 0;
}
