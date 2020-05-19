
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>	  /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define PRINT_ERRNUM fprintf(stderr, "Error at line %d %s\n", __LINE__, strerror(errno));

char shm[14] = "/shm_";
char sem_1[14] = "/sem_";
char sem_2[14] = "/sem_";

char *create_name(char *name, int offset);

// linken nicht vergessen beim Kompillieren!

int main(int argc, char **argv) {	
	int opt = 0; 
	unsigned int length = 0;

	if(argc != 3) { // check Eingabe
        fprintf(stderr, "Usage: %s [-m] size\n", argv[0]);
        exit(EXIT_FAILURE);
    }

	while((opt = getopt(argc, argv, ":m:")) != -1) 
	{ 
		switch(opt) 
		{ 
			case 'm':
			    length = (unsigned int) strtol(optarg, 0, 10);
				break;
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-m] size\n", argv[0]);
				exit(EXIT_FAILURE);
		} 
	}

	if (optind != argc) { // check Eingabe
        fprintf(stderr, "Usage: %s [-m] size\n", argv[0]);
		exit(EXIT_FAILURE);
    }

	(void)create_name(shm, 0);
	(void)create_name(sem_1, 1);
	(void)create_name(sem_2, 2);

	errno = 0;
	sem_t * const p1 = sem_open(sem_1, O_CREAT, S_IRWXU, 0);
	if (p1 == SEM_FAILED) {
		perror("sem_open p1");
		//PRINT_ERRNUM;
		return (EXIT_FAILURE);
	}

	sem_t * const p2 = sem_open(sem_2, O_CREAT, S_IRWXU, 1);
	if (p2 == SEM_FAILED) {
		perror("sem_open p2");
		return (EXIT_FAILURE);
	}

	const int shmfd = shm_open(shm, O_CREAT | O_RDWR, S_IRWXU);
	if (shmfd == -1) {
		PRINT_ERRNUM;
		return (EXIT_FAILURE);
	}

	if (ftruncate(shmfd, length * sizeof(int)) == -1) {
		perror("ftruncate");
		return (EXIT_FAILURE);
	}

	int * const shmptr = mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (shmptr == (int * const) MAP_FAILED) {
		perror("mmap");
	}

	int c = 0;
	int i = 0;

		
	while(1) {
		c = getchar();
		
		(void) sem_wait(p1);

		*(shmptr + i) = c;
		
		(void) sem_post(p2);
		
		//i++;
		//i %= length;
	
		if (c == EOF) {
        	break;
        }	
	
	}
	// Niki:
    // kein sem_unlink(), da das nur ein Prozess machen sollte
    // kein shm_unlink(), da das nur ein Prozess machen sollte
	errno = 0;
	if (close(shmfd) == -1){
		perror("sem_close shmfd");
		return (EXIT_FAILURE);
	}
	//shm_unlink(shm);
	
	if (sem_close(p1) == -1) {
		perror("sem_close p1");
		return (EXIT_FAILURE);
	}
	//sem_unlink(sem_1);

	if (sem_close(p2) == -1) {
		perror("sem_close p2");
		return (EXIT_FAILURE);
	}
	//sem_unlink(sem_2);

	if (munmap(shmptr, sizeof(int)) == -1) {
		perror("munmap shmptr");
		return (EXIT_FAILURE);
	}
	
	
	return 0;
}

char *create_name(char *name, int offset)
{
	uid_t user_id = getuid() * 1000 + offset;
	char uid_string[8];

	snprintf(uid_string, 8, "%d", user_id);
	strcat(name, uid_string);
	name[strlen(name)] = '\0';

	return (EXIT_SUCCESS);
}