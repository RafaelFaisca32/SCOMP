#ifndef COMMON_H
#define COMMON_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Utility functions to manage shared memory areas */
/**
 * @brief Casts PTR to a pointer to TYPE
 */
#define smAs(TYPE, PTR) ((TYPE*)(PTR))
/**
 * @brief Creates a shared memory area if it is not created and maps it to the current address space
 * for writing. Displays an error message and quits if something went wrong.
 *
 * @param name The name of the shared memory area.
 * @param size The target size for the shared memory area.
 * @param fd A pointer to be set to the file descriptor for the shared memory or NULL.
 * @return void* The pointer to the address space where the shared memory was mapped.
 */
void* smOpenForWriting(const char* const name, const size_t size, int* const fd);
/**
 * @brief Opens a shared memory area and maps it to the current address space for reading.
 * Displays an error message and quits if something went wrong.
 *
 * @param name The name of the shared memory area.
 * @param size The target size for the shared memory area.
 * @param fd A pointer to be set to the file descriptor for the shared memory or NULL.
 * @return void* The pointer to the address space where the shared memory was mapped.
 */
void* smOpenForReading(const char* const name, const size_t size, int* const fd);
/**
 * @brief Closes a shared memory area and unmaps it to from the current address space.
 * Displays an error message and quits if something went wrong.
 *
 * @param size The target size for the shared memory area.
 * @param mapedMemory The pointer to the address space where the shared memory was mapped.
 * @param fd A pointer to the file descriptor for the shared memory.
 */
void smClose(const size_t size, void** const mapedMemory, int* const fd);
/**
 * @brief Closes and frees a shared memory area and unmaps it to from the current address space.
 * Displays an error message and quits if something went wrong.
 *
 * @param name he name of the shared memory area.
 * @param size The target size for the shared memory area.
 * @param mapedMemory The pointer to the address space where the shared memory was mapped.
 * @param fd A pointer to the file descriptor for the shared memory.
 */
void smFree(const char* const name, const size_t size, void** const mapedMemory, int* const fd);

/* Utility functions to read from the shared memory area */
/**
 * @brief Copy a single int from the memory and increments the pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @return int The value that was read.
 */
int smReadInt(void** const shm);
/**
 * @brief Copy a single unsigned int from the memory and increments the pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @return unsigned int The value that was read.
 */
unsigned int smReadUInt(void** const shm);
/**
 * @brief Copy a single long from the memory and increments the pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @return long The value that was read.
 */
long smReadLong(void** const shm);
/**
 * @brief Copy a single unsigned long from the memory and increments the pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @return unsigned long The value that was read.
 */
unsigned long smReadULong(void** const shm);
/**
 * @brief Copy a single char from the memory and increments the pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @return char The value that was read.
 */
char smReadChar(void** const shm);
/**
 * @brief Copy a single unsigned char from the memory and increments the pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @return unsigned char The value that was read.
 */
unsigned char smReadUChar(void** const shm);
/**
 * @brief Copy a single nul-ended string from the memory and increments the pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param dest The pointer to the destination where to put the string; or NULL do dynamically
 * allocate.
 * @return char* A pointer to dest if it was not NULL; or a pointer to the allocated memory if dest
 * == NULL; or NULL if dest == NULL and there was a memory allocation error.
 */
char* smReadString(void** const shm, char* const dest);
/**
 * @brief Copy "size" bytes from the memory and increments the pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param dest The pointer to the destination where to put the memory read; or NULL do dynamically
 * allocate.
 * @param size The number of bytes to copy.
 * @return void* A pointer to dest if it was not NULL; or a pointer to the allocated memory if dest
 * == NULL; or NULL if dest == NULL and there was a memory allocation error.
 */
void* smReadBuff(void** const shm, void* const dest, const size_t size);

/* Utility functions to write to the shared memory area */
/**
 * @brief Copy a single int from the shared memory area and increment pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param value The value to write.
 */
void smWriteInt(void** const shm, const int value);
/**
 * @brief Copy a single unsigned int from the shared memory area and increment pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param value The value to write.
 */
void smWriteUInt(void** const shm, const unsigned int value);
/**
 * @brief Copy a single long from the shared memory area and increment pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param value The value to write.
 */
void smWriteLong(void** const shm, const long value);
/**
 * @brief Copy a single unsigned long from the shared memory area and increment pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param value The value to write.
 */
void smWriteULong(void** const shm, const unsigned long value);
/**
 * @brief Copy a single char from the shared memory area and increment pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param value The value to write.
 */
void smWriteChar(void** const shm, const char value);
/**
 * @brief Copy a single unsigned char from the shared memory area and increment pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param value The value to write.
 */
void smWriteUChar(void** const shm, const unsigned char value);
/**
 * @brief Copy a single nul-ended string from the shared memory area and increment pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param source A pointer to the string to copy.
 */
void smWriteString(void** const shm, char* const source);
/**
 * @brief Copy a single "size" bytes from the shared memory area and increment pointer.
 *
 * @param shm The pointer to the memory area to read.
 * @param source A pointer to the bytes to copy.
 * @param size The number of bytes to copy.
 */
void smWriteBuff(void** const shm, void* const source, size_t size);

/* Functions to manage semaphores */
/**
 * @brief Opens a semaphore or exits on error.
 *
 * @param name The name of the semaphore
 * @param initial Iff < 0 then opens an existing semaphore, otherwise creates a
 * semaphore with the indicated value.
 * @return sem_t* The semaphore.
 */
sem_t* semOpen(const char* const name, int initial);

/**
 * @brief Closes a semaphore or exits on error.
 *
 * @param semaphore The semaphore to close.
 */
void semClose(sem_t** const semaphore);

/**
 * @brief Unlinks a semaphore or exits on error.
 *
 * @param name The name of the semaphore.
 */
void semFree(const char* const name);

/**
 * @brief Increments a semaphore or exits on error.
 *
 * @param semaphore The semaphore to increment.
 */
void semUp(sem_t* const semaphore);

/**
 * @brief Decrements a semaphore or exits on error.
 *
 * @param semaphore The semaphore to decrement.
 */
void semDown(sem_t* const semaphore);

/**
 * @brief Gets the value of the semaphore. Displays message and exits on error.
 *
 * @param semaphore The semaphore.
 * @return int The semaphore's value.
 */
int semValue(sem_t* const semaphore);

/**
 * @brief Tries to decrement semaphore without blocking. Displays message and exits on error.
 *
 * @param semaphore The semaphore.
 * @return int Returns 1 if the semaphore was decremented, 0 otherwise.
 */
int semTryDown(sem_t* const semaphore);

#endif
