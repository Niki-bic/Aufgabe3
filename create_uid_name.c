
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

char shm[14] = "/shm_";
char sem_1[14] = "/sem_";
char sem_2[14] = "/sem_";

char *create_name (char *name, int offset);

int main()
{
	(void) create_name(shm, 0);
    (void) create_name(sem_1, 1);
    (void) create_name(sem_2, 2);



	fprintf(stdout, "Shm: %s\n", shm);
    fprintf(stdout, "Sem_1: %s\n", sem_1);
    fprintf(stdout, "Sem_2: %s\n", sem_2);

	return 0;
}

char *create_name (char *name, int offset) {
	uid_t user_id = getuid() * 1000 + offset;	
	char uid_string[8];

	snprintf(uid_string, 8, "%d", user_id);
	strcat(name, uid_string);
    name[strlen(name)] = '\0';

	return (EXIT_SUCCESS);
}