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


char shm_name_0[14];
char sem_name_1[14];
char sem_name_2[14];
char sem_name_3[14];


void close_all(int shared_memory, sem_t *sem_mutex, sem_t *sem_full, sem_t *sem_empty);
void unlink_sem(char *sem_name_1, char *sem_name_2, char *sem_name_3);
void make_names(void);
char *generate_name(char *name, uid_t id, const int offset);
char *reverse_string(char *string);
sem_t *sem_open_error_checked(const char *name, int oflag, mode_t mode, unsigned int value);
unsigned int arguments(int argc, char **argv);


#endif


