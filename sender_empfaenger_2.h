#ifndef SENDER_EMPFAENGER_H_2
#define SENDER_EMPFAENGER_H_2

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define TRUE 1


struct resources{
    int argc;
    char **argv;
    sem_t *sem_full;
    sem_t *sem_empty;
    int *shared_mem_pointer;
    int shared_memory;
    unsigned long length;
    char shm_name_0[14];
    char sem_name_1[14];
    char sem_name_2[14];
};


void init_resources(int argc, char **argv, struct resources *r);
void check_arguments(struct resources *r);
long strtol_errorchecked(const char * const string, struct resources *r);
void create_name(char *name, unsigned int offset, struct resources *r);
sem_t *sem_open_errorchecked(const char *name, int oflag, mode_t mode, \
        unsigned int value, struct resources *r);
int shm_open_errorchecked(const char *name, int oflag, mode_t mode, struct resources *r);
void ftruncate_errorchecked(int fd, off_t length, struct resources *r);
int *mmap_errorchecked(void *addr, size_t length, int prot, int flags, \
        int fd, off_t offset, struct resources *r);
void printf_errorchecked(FILE *stream, const char * const string, ...);
void remove_resources(int exit_status, struct resources *r);


#endif


