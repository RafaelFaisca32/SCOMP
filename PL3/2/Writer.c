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
        void *   mmapres, *write;
        int      price_eur, fd;
        unsigned price_cent, productCode;
        char     desc[BUFF_SIZE - (sizeof(int) + sizeof(unsigned) * 2)];

        /* Open shared memory */
        mmapres = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &fd);

        /* Read input from user */
        do {
                setbuf(stdin, NULL);
                printf("PLEASE ENTER THE PRINCE IN THE FORMAT XX.XX > ");
        } while (scanf("%d.%u", &price_eur, &price_cent) != 2 || price_cent > 99);

        do {
                setbuf(stdin, NULL);
                printf("PLEASE ENTER THE NUMERICAL PRODUCT CODE > ");
        } while (scanf("%u", &productCode) != 1);

        do {
                setbuf(stdin, NULL);
                printf("PLEASE ENTER THE PRODUCT DESCRIPTION > ");
        } while (fgets(desc, sizeof(desc), stdin) != desc);


        /* Write data to shared memory */
        printf("WRITING DATA...\n");
        write = mmapres;
        smWriteInt(&write, price_eur);
        smWriteUInt(&write, price_cent);
        smWriteUInt(&write, productCode);
        smWriteString(&write, desc);

        /* Close shared memory */
        smClose(BUFF_SIZE, &mmapres, &fd);

        printf("DONE\n");
        return 0;
}
