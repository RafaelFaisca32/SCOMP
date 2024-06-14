#ifndef CONSTS_H
#define CONSTS_H

struct Ex10 {
        int  number;
        char name[50];
        char address[50];
};

#define SHARED_MEMORY_NAME "/pl4ex10"
#define BUFF_SIZE          sizeof(struct Ex10) * RECORD_SIZE
#define RECORD_SIZE        100
#define SEM_NAME1          "/sem1"
#define SEM_NAME2          "/sem2"

#endif
