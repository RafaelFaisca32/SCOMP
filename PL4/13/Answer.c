#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"

#define SEM_NAME_S         "/pl4ex11.s"
#define SEM_NAME_C         "/pl4ex11.c"
#define SEM_NAME_M         "/pl4ex11.m"
#define SHARED_MEMORY_NAME "/pl4ex11"
#define SPACING            "                    "
#define BUFF_SIZE          sizeof(circular_buffer)
#define TYPE               int
#define NUMS_TO_EXCHANGE   30
#define ARR_SIZE           10
#define CHLD_N             2

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
        /**
         * @brief The total number of elements written to the buffer;
         */
        size_t totalExchanged;
} circular_buffer;

/**
 * @brief Initializes the circular buffer.
 *
 * @param b The circular_buffer.
 */
void circular_buffer_new(volatile circular_buffer* b) {
        b->start          = 0;
        b->size           = 0;
        b->totalExchanged = 0;
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
                b->totalExchanged++;
                return 1;
        }
}

int main() {
        pid_t                     p[CHLD_N];
        void*                     shm;
        volatile circular_buffer* b;
        int                       smfd;
        int                       i, k;
        /* Represent the size, capacity and a mutex for the buffer*/
        sem_t *semSiz, *semCap, *semMutex;

        /* Open shared memory area */
        shm = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);
        b   = shm;
        circular_buffer_new(b);

        /* Open semaphores */
        semSiz   = semOpen(SEM_NAME_S, 0);
        semCap   = semOpen(SEM_NAME_C, ARR_SIZE);
        semMutex = semOpen(SEM_NAME_M, 1);
        printf("     CHILD      ---      PARENT\n");
        fflush(stdout);

        for (k = 0; k < CHLD_N; k++) {
                /* Fork (create new process) */
                p[k] = fork();
                if (p[k] == -1) {
                        /* Fork error */
                        fprintf(stderr, "Could not fork.\n");
                        /* Free shared memory */
                        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &shm, &smfd);
                        exit(EXIT_FAILURE);
                } else if (p[k] == 0) {
                        /* Child (producer) */

                        /* Open shared memory */
                        shm = smOpenForWriting(SHARED_MEMORY_NAME, BUFF_SIZE, &smfd);
                        b   = shm;

                        /* Open semaphores */
                        semSiz   = semOpen(SEM_NAME_S, -1);
                        semCap   = semOpen(SEM_NAME_C, -1);
                        semMutex = semOpen(SEM_NAME_M, -1);

                        /* Produce */
                        for (i = 1;; i++) {
                                /* Ensure buffer is not full*/
                                semDown(semCap);
                                /* Request access to the buffer */
                                semDown(semMutex);
                                /* Check if necessary number already exchanged */
                                if (b->totalExchanged >= NUMS_TO_EXCHANGE) {
                                        semUp(semMutex);
                                        break;
                                }
                                /* Try to push i into the buffer */
                                if (circular_buffer_push(b, i + (k + 1) * 100)) {
                                        /* Was able to push */
                                        printf("[%.3d] SET: %.3d\n", k, i + (k + 1) * 100);
                                        fflush(stdout);
                                        semUp(semSiz);
                                } else {
                                        /* Buffer is full, retry this number again */
                                        i--;
                                        semUp(semCap);
                                }
                                /* Give up access to the buffer */
                                semUp(semMutex);
                        }

                        /* Close shared memory */
                        smClose(BUFF_SIZE, &shm, &smfd);

                        /* Close semaphores */
                        semClose(&semSiz);
                        semClose(&semCap);
                        semClose(&semMutex);

                        /* Exit */
                        printf("CHILD DONE\n");
                        fflush(stdout);
                        exit(EXIT_SUCCESS);
                }
        }
        /* Parent (Conssumer) */

        /* Consume */
        for (i = 0; i < 30;) {
                /* Ensure buffer is not full*/
                semDown(semSiz);
                /* Request access to the buffer */
                semDown(semMutex);
                /* Try to pop i from the buffer */
                if (circular_buffer_pop(b, &k)) {
                        /* Was able to pop into k */
                        i++;
                        printf(SPACING "GET: [%.3d] -> %.3d\n", i, k);
                        semUp(semCap);
                        fflush(stdout);
                } else {
                        semUp(semSiz);
                }

                /* Give up access to the buffer */
                semUp(semMutex);
        }


        /* Wait for children */
        printf(SPACING "WAITING FOR CHILDREN...\n");
        fflush(stdout);
        for (i = 0; i < CHLD_N; i++) waitpid(p[i], NULL, 0);

        /* Free shared memory */
        smFree(SHARED_MEMORY_NAME, BUFF_SIZE, &shm, &smfd);

        /* Close semaphores */
        semClose(&semSiz);
        semClose(&semCap);
        semClose(&semMutex);

        /* Free semaphores */
        semFree(SEM_NAME_S);
        semFree(SEM_NAME_C);
        semFree(SEM_NAME_M);

        /* Exit */
        printf(SPACING "PARENT DONE\n");
        fflush(stdout);

        return EXIT_SUCCESS;
}
