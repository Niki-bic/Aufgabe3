#ifndef SENDER_EMPFAENGER_H
#define SENDER_EMPFAENGER_H


#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>


#define NAME_LEN 14


struct resources{
    int argc;
    char * const *argv;
    sem_t *sem_full;
    sem_t *sem_empty;
    int *shared_mem_pointer;
    int shared_memory;
    unsigned long length;
    char shm_name_0[NAME_LEN];
    char sem_name_1[NAME_LEN];
    char sem_name_2[NAME_LEN];
};

void init_resources(const int argc, const char * const * const argv, struct resources * const r);
void printf_errorchecked(FILE * const stream, const char * const string, ...);
void remove_resources(int exit_status, struct resources * const r);
void sem_wait_errorchecked(sem_t * const sem, struct resources * const r);
void sem_post_errorchecked(sem_t * const sem, struct resources * const r);


#endif
