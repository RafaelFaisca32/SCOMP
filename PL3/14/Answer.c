#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "common.h"

#define SHARED_MEMORY_NAME "/pl3ex14"
#define SPACE              "           "
#define BUFF_SIZE          sizeof(circular_buffer)
#define TYPE               int
#define NUMS_TO_EXCHANGE   30
#define ARR_SIZE           10

/**
 * @brief Represents an access key that is unassigned.
 */
#define ACCESS_NONE 0
/**
 * @brief Represents an access key that is assigned to the producer.
 */
#define ACCESS_PRODUCER 1
/**
 * @brief Represents an access key that is assigned to the consumer.
 */
#define ACCESS_CONSUMER 2
/**
 * @brief Represents the value after the last access key.
 */
#define ACCESS_MAX 3

/**
 * @brief Represents a circular buffer.
 */
typedef struct {
        /**
         * @brief The array in the circular buffer.
         */
        TYPE arr[ARR_SIZE];
        /**
         * @brief The first element in the buffer.
         */
        size_t start;
        /**
         * @brief The number of elements in the buffer.
         */
        size_t size;
        /* Used for synchronizing */
        int access1;
        int access2;
} circular_buffer;

/**
 * @brief Initializes the circular buffer.
 *
 * @param b The circular_buffer.
 */
void circular_buffer_new(volatile circular_buffer* b) {
        b->start   = 0;
        b->size    = 0;
        b->access1 = ACCESS_NONE;
        b->access2 = ACCESS_NONE;
}

/**
 * @brief Pop a value onto the circular buffer.
 *
 * @param b The circular_buffer.
 * @param v A pointer to the destination of the popped value.
 * @return int Returns 1 iff the value was popped.
 */
int circular_buffer_pop(volatile circular_buffer* b, TYPE* v) {
        if (b->size == 0) {
                return 0;
        } else {
                *v = b->arr[b->start];
                b->start++;
                if (b->start == ARR_SIZE) b->start = 0;
                b->size--;
                return 1;
        }
}

/**
 * @brief Push a value onto the circular buffer.
 *
 * @param b The circular_buffer.
 * @param v The value to push.
 * @return int Returns 1 iff the value was pushed.
 */
int circular_buffer_push(volatile circular_buffer* b, const TYPE v) {
        if (b->size == ARR_SIZE) {
                return 0;
        } else {
                b->arr[(b->start + b->size) % ARR_SIZE] = v;
                b->size++;
                return 1;
        }
}

/**
 * @brief Simulates a mutex.(?)
 *
 * @param b The circular_buffer.
 * @param access The acess "key".
 */
void circular_buffer_request(volatile circular_buffer* b, int access) {
        do {
                if (b->access1 == access || b->access1 <= ACCESS_NONE || b->access1 >= ACCESS_MAX) {
                        b->access1 = access;
                        if (b->access1 == access && b->access2 != access) { b->access2 = access; }
                }
        } while (b->access1 != access || b->access2 != access);
}

/**
 * @brief Give up acess to the circular buffer.
 *
 * @param b The circular_buffer.
 */
void circular_buffer_giveup(volatile circular_buffer* b) {
        b->access1 = ACCESS_NONE;
        b->access2 = ACCESS_NONE;
}

int main() {
        pid_t                     p;
        void*                     shm;
        volatile circular_buffer* b;
        int                       smfd;
        int                       i;

        /* Open shared memory area */
        shm = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);
        b   = shm;
        circular_buffer_new(b);

        printf("PARENT  -  CHILD\n");
        fflush(stdout);

        /* Fork (create new process) */
        p = fork();
        if (p == -1) {
                /* Fork error */
                fprintf(stderr, "Could not fork.\n");
                /* Free shared memory */
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &shm, &smfd);
                exit(EXIT_FAILURE);
        } else if (p == 0) {
                /* Child (CONSUMER) */
                size_t j = 0;

                /* Open shared memory */
                shm = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);
                b   = shm;

                /* Consume */
                for (i = 0; i != NUMS_TO_EXCHANGE;) {
                        /* Request access to the buffer */
                        circular_buffer_request(b, ACCESS_CONSUMER);
                        /* Try to pop i from the buffer */
                        if (circular_buffer_pop(b, &i)) {
                                /* Was able to pop i */
                                printf(SPACE "[%lu] GET: %d\n", ++j, i);
                                fflush(stdout);
                        }
                        /* Give up access to the buffer */
                        circular_buffer_giveup(b);
                }

                /* Close shared memory */
                smClose(BUFF_SIZE, &shm, &smfd);

                /* Exit */
                printf(SPACE "CHILD DONE\n");
                fflush(stdout);
                exit(EXIT_SUCCESS);
        } else {
                /* Parent (PRODUCER) */

                /* Produce */
                for (i = 1; i <= NUMS_TO_EXCHANGE; i++) {
                        /* Request access to the buffer */
                        circular_buffer_request(b, ACCESS_PRODUCER);
                        /* Try to push i into the buffer */
                        if (circular_buffer_push(b, i)) {
                                /* Was able to push */
                                printf("SET: %d\n", i);
                                fflush(stdout);
                        } else {
                                /* Buffer is full, retry this number again */
                                i--;
                        }
                        /* Give up access to the buffer */
                        circular_buffer_giveup(b);
                }


                /* Wait for child */
                printf("WAITING FOR CHILD...\n");
                fflush(stdout);
                waitpid(p, NULL, 0);

                /* Free shared memory */
                smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &shm, &smfd);

                /* Exit */
                printf("PARENT DONE\n");
                fflush(stdout);
                exit(EXIT_SUCCESS);
        }

        return 0;
}
