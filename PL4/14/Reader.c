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
        int    smfd, verifica = 0;
        void*  mmapres;
        sem_t *wmutex, *rmutex, *killSMSEM;

        /* Create shared memory */
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        wmutex    = semOpen(SEMW, 1);
        rmutex    = semOpen(SEMR, 1);
        killSMSEM = semOpen(SEMK, 0);

        /* este semáforo é utilizado para bloquear o acesso ao free dos semáforos enquanto todos as
         * instancias do reader estiverem a ser executadas */
        semUp(killSMSEM);

        /* este while serve para confirmar se o Writer ja terminou */
        while (verifica <= 0) {
                verifica = semValue(wmutex); /* se o valor deste semáforo = 0, significa que o
                                                Writer ainda não terminou */
                if (verifica <= 0) sleep(1);
        }

        /* processo está no Reader */
        semDown(rmutex);

        smAs(struct Ex14, mmapres)->active_readers++;
        printf("Readers working at the moment: %d\tWriters working at the moment: %d\n",
               smAs(struct Ex14, mmapres)->active_readers,
               smAs(struct Ex14, mmapres)->active_writers);

        /* processo no Reader terminou */
        semUp(rmutex);

        semDown(killSMSEM);

        if (semValue(killSMSEM) == 0) {
                /* free aos semaforos */
                semFree(SEMW);
                semFree(SEMR);
                semFree(SEMK);

                /* Close shared memory area */
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);
        }

        return 0;
}
