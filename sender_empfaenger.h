#ifndef SENDER_EMPFAENGER_H
#define SENDER_EMPFAENGER_H


#include <fcntl.h>
#include "id_to_name.h"
#include <math.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


uid_t id = getuid();
const char *sem_name_1 = id_to_name("/sem_", id, "1");
const char *shm_name_1 = id_to_name("/shm_", id, "1");


#endif
