#ifndef SENDER_EMPFAENGER_H
#define SENDER_EMPFAENGER_H


#include "generate_name.h"
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


char shm_name_0[14] = "/shm_"; // name for shared memory
char sem_name_1[14] = "/sem_"; // name for semaphore mutex
char sem_name_2[14] = "/sem_"; // name for semaphore count of full entries
char sem_name_3[14] = "/sem_"; // name for semaphore count of empty spaces


#endif
