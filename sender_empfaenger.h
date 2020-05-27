#ifndef SENDER_EMPFAENGER_H
#define SENDER_EMPFAENGER_H

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define TRUE 1


char shm_name_0[14]; // name for shared-memory
char sem_name_1[14]; // name for semaphore-full
char sem_name_2[14]; // name for semaphore-empty 


char *generate_name(char *name, uid_t id, const int offset);
char *reverse_string(char *string);
int shm_open_error_checked(const char *name, int oflag, mode_t mode);
unsigned int arguments(int argc, char **argv);
void close_all(int shared_memory, sem_t *sem_full, sem_t *sem_empty);
void ftruncate_error_checked(int fd, off_t length);
void make_names(void);
void *mmap_error_checked(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
void unlink_all_sem(char *sem_name_1, char *sem_name_2);
sem_t *sem_open_error_checked(const char *name, int oflag, mode_t mode, unsigned int value);


#endif


