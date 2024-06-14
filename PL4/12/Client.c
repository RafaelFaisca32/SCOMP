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
        void * mmapres, *read;
        int    smfd, bilhete;
        sem_t *sem1, *sem2;

        /* open à leitura e escrita na shared memory*/
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* open do semáforo */
        sem1 = semOpen(SEM_NAME1, 0);

        /* open do semáforo */
        sem2 = semOpen(SEM_NAME2, 0);


        /* semáforo que bloqueia a saida no cliente */
        semDown(sem1);

        read    = mmapres;
        bilhete = smReadInt(&read);
        sleep(2);

        /* semáforo que confirma que o bilhete foi dado */
        semUp(sem2);

        printf("Número do bilhete é : %d\n", bilhete);

        /* Close shared memory area */
        smClose(BUFF_SIZE, &mmapres, &smfd);

        return 0;
}
