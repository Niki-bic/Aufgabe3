#ifndef ID_TO_NAME_H
#define ID_TO_NAME_H

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


char *generate_name(char *prefix, uid_t id, int offset);
char *reverse_string(char *string);


#endif