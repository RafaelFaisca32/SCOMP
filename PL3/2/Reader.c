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
        int      smfd;
        void *   mmapres, *read;
        int      price_eur;
        unsigned price_cent, productCode;
        char     desc[BUFF_SIZE - (sizeof(int) + sizeof(unsigned) * 2)];

        mmapres = smOpenForReading(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);

        /* Read data from shared memory */
        printf("READING DATA...\n");

        read        = mmapres;
        price_eur   = smReadInt(&read);
        price_cent  = smReadUInt(&read);
        productCode = smReadUInt(&read);
        smReadString(&read, desc);

        printf("Price: %d.%u\nProduct code: %u\nProduct description: %s", price_eur, price_cent,
               productCode, desc);

        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &mmapres, &smfd);

        printf("DONE\n");
        return 0;
}
