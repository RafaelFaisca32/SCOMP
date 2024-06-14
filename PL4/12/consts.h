#ifndef CONSTS_H
#define CONSTS_H

struct Ex12 {
        int next_ticket;
        int number_tickets;
};

#define SHARED_MEMORY_NAME "/pl4ex12"
#define BUFF_SIZE          sizeof(struct Ex12)
#define SEM_NAME1          "/sem1"
#define SEM_NAME2          "/sem2"
#define SEM_NAME3          "/sem3"
#define SEM_NAME4          "/sem4"

#endif
