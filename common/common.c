#include "common.h"

/**
 * @brief Unlinks the shared memory area or displays error and quits.
 *
 * @param name The name of the shared memory area.
 */
void smUnlinkOrError(const char* const name) {
        /* Unlink shared memory in the OS */
        if (shm_unlink(name) == -1) {
                fprintf(stderr, "Could not close shared memory in the file system: [%s]\n",
                        strerror(errno));
                exit(EXIT_FAILURE);
        }
}

/**
 * @brief Closes the shared memory area or displays error and quits.
 *
 * @param fd The file descriptor.
 */
void smCloseOrError(int* const fd) {
        /* Close shared memory area */
        if (close(*fd) == -1) {
                fprintf(stderr, "Could not close shared memory: [%s]\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
        /* For safety */
        *fd = -1;
}

/**
 * @brief Unmaps the shared memory from the address space or displays error and quits.
 *
 * @param size Size of the shared memory.
 * @param mapedMemory Pointer to the shared memory address in memory.
 */
void smMunmapOrError(const size_t size, void** const mapedMemory) {
        /* Unmap shared memory from address space */
        if (munmap((*mapedMemory), size) == -1) {
                fprintf(stderr, "Could not unmap shared memory: [%s]\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
        /* For safety */
        *mapedMemory = NULL;
}

/**
 * @brief Creates a shared memory area or displays error and quits.
 *
 * @param name The name of the shared memory area.
 * @return int The file descriptor to the shared memory.
 */
int smCreateOrError(const char* const name) {
        int smfd;
        /* Open shared memory area */
        if ((smfd = shm_open(name, O_CREAT | O_RDWR, S_IRWXU)) == -1) {
                fprintf(stderr, "Could not create shared memory area. [%s]\n", name);
                exit(EXIT_FAILURE);
        }
        return smfd;
}

/**
 * @brief Opens a shared memory area or displays error and quits.
 *
 * @param name The name of the shared memory area.
 * @return int The file descriptor to the shared memory.
 */
int smOpenOrError(const char* const name) {
        int smfd;
        /* Open shared memory area */
        if ((smfd = shm_open(name, O_RDONLY, S_IRWXU)) < 0) {
                fprintf(stderr, "Could not open shared memory area. [%s]\n", name);
                exit(EXIT_FAILURE);
        }
        return smfd;
}

/**
 * @brief Changes the size of the shared memory area or displays error and quits.
 *
 * @param name The name of the shared memory area.
 * @param size The target size for the shared memory area.
 * @param fd The file descriptor for the shared memory.
 */
void smTruncateOrError(const char* const name, const size_t size, const int fd) {
        /* Truncate shared memory to trget size */
        if (ftruncate(fd, size) == -1) {
                fprintf(stderr, "Could not truncate shared memory to size %lu: [%s]\n",
                        (long unsigned)size, strerror(errno));
                smUnlinkOrError(name);
                exit(EXIT_FAILURE);
        }
}

/**
 * @brief Maps the shared memory to the address space or displays error and quits.
 *
 * @param name The name of the shared memory area.
 * @param size The target size for the shared memory area.
 * @param fd The file descriptor for the shared memory.
 * @param flags the flags for the mapped shared memory.
 * @return void* The pointer to the address space where the shared memory was mapped.
 */
void* smMapOrError(const char* const name, const size_t size, int* const fd, const int flags) {
        void* mmapres;
        /* Map shared memory to processes addressingn space */
        if ((mmapres = mmap(NULL, size, flags, MAP_SHARED, *fd, 0)) == MAP_FAILED) {
                fprintf(stderr, "Could not map shared memory to address space: [%s]\n",
                        strerror(errno));
                smCloseOrError(fd);
                smUnlinkOrError(name);
                exit(EXIT_FAILURE);
        }
        return mmapres;
}

void* smOpenForWriting(const char* const name, const size_t size, int* const fd) {
        int   smfd;
        void* mmapres;

        /* Open shared memory area */
        smfd = smCreateOrError(name);

        /* Truncate shared memory to trget size */
        smTruncateOrError(name, size, smfd);

        /* Map shared memory to processes addressingn space */
        mmapres = smMapOrError(name, size, &smfd, PROT_WRITE | PROT_READ);

        if (fd != NULL) *fd = smfd;
        return mmapres;
}

void* smOpenForReading(const char* const name, const size_t size, int* const fd) {
        int   smfd;
        void* mmapres;

        /* Open shared memory */
        smfd = smOpenOrError(name);

        /* No need for ftruncate, size is already known to OS */

        /* Map shared memory to address space */
        mmapres = smMapOrError(name, size, &smfd, PROT_READ);

        if (fd != NULL) *fd = smfd;
        return mmapres;
}

void smClose(const size_t size, void** const mapedMemory, int* const fd) {
        /* Unmap shared memory from address space */
        smMunmapOrError(size, mapedMemory);

        /* Close shared memory area */
        smCloseOrError(fd);
}

void smFree(const char* const name, const size_t size, void** const mapedMemory, int* const fd) {
        /* Unmap shared memory from address space & close shared memory area */
        smClose(size, mapedMemory, fd);

        /* Unlink shared memory in the OS */
        smUnlinkOrError(name);
}

/**
 * @brief A useful macro to easily define basic functions.
 */
#define _COMMON_READ_(FUNC_NAME, TYPE)                                                             \
        TYPE FUNC_NAME(void** const shm) {                                                         \
                TYPE value;                                                                        \
                value  = *((TYPE*)(*shm));                                                         \
                (*shm) = ((char*)(*shm)) + sizeof(TYPE);                                           \
                return value;                                                                      \
        }
/* Define the most basic functions. */
_COMMON_READ_(smReadInt, int);
_COMMON_READ_(smReadUInt, unsigned int);
_COMMON_READ_(smReadLong, long);
_COMMON_READ_(smReadULong, unsigned long);
_COMMON_READ_(smReadChar, char);
_COMMON_READ_(smReadUChar, unsigned char);

char* smReadString(void** const shm, char* const dest) {
        const size_t len = strlen((char*)(*shm));
        if (dest != NULL) {
                strcpy(dest, (char*)(*shm));
                (*shm) = (*shm) + len + 1;
                return dest;
        } else {
                char* alloc = (char*)malloc(len + 1);
                if (alloc == NULL) return NULL;
                strcpy(alloc, (char*)(*shm));
                (*shm) = (*shm) + len + 1;
                return alloc;
        }
}

void* smReadBuff(void** const shm, void* const dest, const size_t size) {
        if (dest != NULL) {
                memcpy(dest, (char*)(*shm), size);
                (*shm) = (*shm) + size;
                return dest;
        } else {
                void* alloc = malloc(size);
                if (alloc == NULL) return NULL;
                memcpy(alloc, (char*)(*shm), size);
                (*shm) = (*shm) + size;
                return alloc;
        }
}

/**
 * @brief A useful macro to easily define basic functions.
 */
#define _COMMON_WRITE_(FUNC_NAME, TYPE)                                                            \
        void FUNC_NAME(void** const shm, const TYPE value) {                                       \
                *((TYPE*)(*shm)) = value;                                                          \
                (*shm)           = ((char*)(*shm)) + sizeof(TYPE);                                 \
        }

/* Define the most basic functions. */
_COMMON_WRITE_(smWriteInt, int);
_COMMON_WRITE_(smWriteUInt, unsigned int);
_COMMON_WRITE_(smWriteLong, long);
_COMMON_WRITE_(smWriteULong, unsigned long);
_COMMON_WRITE_(smWriteChar, char);
_COMMON_WRITE_(smWriteUChar, unsigned char);

void smWriteString(void** const shm, char* const source) {
        strcpy((char*)(*shm), source);
        (*shm) = (*shm) + strlen((char*)(*shm)) + 1;
}

void smWriteBuff(void** const shm, void* const source, size_t size) {
        memcpy((char*)(*shm), source, size);
        (*shm) = (*shm) + size;
}


sem_t* semOpen(const char* const name, int initial) {
        sem_t* v;

        /* Open/create semaphore */
        if (initial >= 0) {
                v = sem_open(name, O_CREAT, 0644, (unsigned)initial);
        } else {
                v = sem_open(name, 0);
        }

        /* Check for error */
        if (v == SEM_FAILED) {
                fprintf(stderr, "Could not open semaphore: [%s]\n", strerror(errno));
                exit(EXIT_FAILURE);
        }

        /* Return */
        return v;
}

void semClose(sem_t** const semaphore) {
        /* Check for error */
        if (sem_close(*semaphore) == -1) {
                fprintf(stderr, "Could not close semaphore: [%s]\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
        /* For safety */
        *semaphore = NULL;
}

void semFree(const char* const name) {
        /* Check for error */
        if (sem_unlink(name) == -1) {
                fprintf(stderr, "Could not free semaphore: [%s]\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
}

void semUp(sem_t* const semaphore) {
        /* Check for error */
        if (sem_post(semaphore) == -1) {
                fprintf(stderr, "Could not up semaphore: [%s]\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
}

void semDown(sem_t* const semaphore) {
        /* Check for error */
        if (sem_wait(semaphore) == -1) {
                fprintf(stderr, "Could not down semaphore: [%s]\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
}

int semTryDown(sem_t* const semaphore) {
        /* Check for error */
        if (sem_trywait(semaphore) == -1) {
                /* Could not lock semaphore */
                if (errno == EAGAIN) {
                        /* Semaphore was already locked (had value 0) */
                        return 0;
                } else {
                        /* There was another error */
                        fprintf(stderr, "Could not down semaphore: [%s]\n", strerror(errno));
                        exit(EXIT_FAILURE);
                }
        } else {
                /* Semaphore was locked */
                return 1;
        }
}

int semValue(sem_t* const semaphore) {
        int v;
        if (sem_getvalue(semaphore, &v) == -1) {
                fprintf(stderr, "Could not get value of semaphore: [%s]\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
        return v;
}