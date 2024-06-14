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
        void*       mmapres;
        int*        write;
        int         smfd;
        struct Ex12 tickets;
        sem_t *     sem1, *sem2;

        /* open à leitura e escrita na shared memory*/
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* open do semáforo */
        sem1 = semOpen(SEM_NAME1, 0);

        /* open do semáforo */
        sem2 = semOpen(SEM_NAME2, 0);

        tickets.number_tickets = 5;
        tickets.next_ticket    = 1;

        write = mmapres;

        do {
                *write = tickets.next_ticket;
                tickets.number_tickets--;
                tickets.next_ticket++;

                /* semáforo que desbloqueia o acesso ao cliente */
                semUp(sem1);

                printf("A atender o cliente %d\n", tickets.next_ticket - 1);

                /* semáforo que espera que o bilhete seja entregue ao cliente */
                semDown(sem2);

        } while (tickets.number_tickets >= 1);


        /* Close shared memory area */
        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);

        printf("A sair...\n");

        /* Free semaphore */
        semFree(SEM_NAME1);

        /* Free semaphore */
        semFree(SEM_NAME2);

        return 0;
}
