#ifndef CONSTS_H
#define CONSTS_H

#define SHARED_MEMORY_NAME "/pl4ex14"
#define SEMW               "/semw"
#define SEMR               "/semr"
#define SEMK               "/semK"
#define BUFF_SIZE          sizeof(struct Ex14) * 10

struct Ex14 {
        char str[100];
        int  active_readers;
        int  active_writers;
};

#endif
