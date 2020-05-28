#include "sender_empfaenger_2.h"


void init_resources(const int argc, const char * const * const argv, struct resources *r) {
    r->argc = (int) argc;
    r->argv = (char **) argv;

    check_arguments(r);

    strcat(r->shm_name_0, "/shm_");
    strcat(r->sem_name_1, "/sem_");
    strcat(r->sem_name_2, "/sem_");

	create_name(r->shm_name_0, 0, r);
    create_name(r->sem_name_1, 1, r);
    create_name(r->sem_name_2, 2, r);

    r->sem_full  = sem_open_errorchecked(r->sem_name_1, O_CREAT, S_IRWXU, 0, r);
    r->sem_empty = sem_open_errorchecked(r->sem_name_2, O_CREAT, S_IRWXU, r->length, r);

    if (strcmp(argv[0], "./sender") == 0) {
        r->shared_memory = shm_open_errorchecked(r->shm_name_0, O_CREAT | O_RDWR, S_IRWXU, r);
        ftruncate_errorchecked(r->shared_memory, r->length * sizeof(int), r);
        r->shared_mem_pointer = mmap_errorchecked(NULL, r->length * sizeof(int), \
                PROT_WRITE, MAP_SHARED, r->shared_memory, 0, r);
    }
    else if (strcmp(argv[0], "./empfaenger") == 0) {
        r->shared_memory = shm_open_errorchecked(r->shm_name_0, O_CREAT | O_RDONLY, S_IRWXU, r);
        ftruncate_errorchecked(r->shared_memory, r->length * sizeof(int), r);
            r->shared_mem_pointer = mmap_errorchecked(NULL, r->length * sizeof(int), \
                PROT_READ, MAP_SHARED, r->shared_memory, 0, r);
    }
    else {
        // error
    }
} // end init_resources


void check_arguments(struct resources * const r) {
    int opt;
	
	if(r->argc != 3) { // check Eingabe
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

    while ((opt = getopt(r->argc, r->argv, ":m:")) != -1) {
        switch (opt) {
            case 'm':
                r->length = (unsigned long) strtol_errorchecked(optarg, r);
                break;
            default:
                printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
                remove_resources(EXIT_FAILURE, r);
        }
    }
	
    if (optind != r->argc) { // check Eingabe
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }
} // end check_arguments


long strtol_errorchecked(const char * const string, struct resources *r){
	char *end_ptr;
	errno = 0;
	long number = strtol(string, &end_ptr, 10);

	if (*end_ptr != '\0' || errno != 0) {
        printf_errorchecked(stderr, "%s: Error in strtol\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
	}
    else if (number <= 0 || number >= 2147483648) {
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

	return number;
} // end strtol_errorchecked


void create_name(char *name, const unsigned int offset, struct resources *r) {
	uid_t user_id = getuid() * 1000 + offset; // This function is always successfull
	char uid_string[8];

	if (snprintf(uid_string, 8, "%d", user_id) < 0) {
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }
        
	strcat(name, uid_string);
    name[strlen(name)] = '\0';
} // end create_name
    

sem_t *sem_open_errorchecked(const char *name, const int oflag, const mode_t mode, \
        const unsigned int value, struct resources *r) {
    sem_t *sem_pointer = sem_open(name, oflag, mode, value);

    if (sem_pointer == SEM_FAILED) {
        printf_errorchecked(stderr, "%s: Error in sem_open\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

    return sem_pointer;
} // end sem_open_errorchecked


int shm_open_errorchecked(const char *name, const int oflag, const mode_t mode, struct resources *r) {
    int shared_memory = shm_open(name, oflag, mode);

    if (shared_memory == -1) {
        printf_errorchecked(stderr, "%s: Error in shm_open\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

    return shared_memory;
} // end shm_open_errorchecked


void ftruncate_errorchecked(int fd, const off_t length, struct resources *r) {
    if (ftruncate(fd, length) == -1) {
        // error
    }
} // end ftruncate_errorchecked


int *mmap_errorchecked(void *addr, const size_t length, const int prot, const int flags, \
        const int fd, const off_t offset, struct resources *r) {
    void *shared_mem_pointer = mmap(addr, length, prot, flags, fd, offset);
    
    if (shared_mem_pointer == MAP_FAILED) {
        printf_errorchecked(stderr, "%s: Error in mmap\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

    return shared_mem_pointer;
} // end mmap_errorchecked


void printf_errorchecked(FILE * const stream, const char * const string, ...){
	va_list array;
	va_start(array, string);

	if(vfprintf(stream, string, array) < 0){
		fprintf(stderr, "Error in vfprintf()!\n");
		exit(EXIT_FAILURE);
	}

	va_end(array);
} // end printf_errorchecked


void remove_resources(int exit_status, struct resources *r){
    if (r->sem_full != NULL) {
        if (sem_close(r->sem_full) == -1) {
            printf_errorchecked(stderr, "%s: Error in sem_close\n", r->argv[0]);
            exit_status = EXIT_FAILURE;
        }
    }

    if (r->sem_empty != NULL) {
        if (sem_close(r->sem_empty) == -1) {
            printf_errorchecked(stderr, "%s: Error in sem_close\n", r->argv[0]);
            exit_status = EXIT_FAILURE;
        }
    }

    if (r->shared_memory != 0) {
        if (close(r->shared_memory) == -1) {
            printf_errorchecked(stderr, "%s: Error in close\n", r->argv[0]);
            exit_status = EXIT_FAILURE;
        }
    }

    if (r->shared_mem_pointer != NULL) {
        if (munmap(r->shared_mem_pointer, r->length * sizeof(int)) == -1) {
            printf_errorchecked(stderr, "%s: Error in munmap\n", r->argv[0]);
            exit_status = EXIT_FAILURE;
        }
    }


    // Nur der empfaenger oder im Fehlerfall soll ungelinkt werden
    if (strcmp(r->argv[0], "./empfaenger") == 0 || exit_status == EXIT_FAILURE) {
        if (r->shm_name_0 != NULL) {
            if (shm_unlink(r->shm_name_0) == -1) {
                printf_errorchecked(stderr, "%s: Error in shm_unlink\n", r->argv[0]);
                exit_status = EXIT_FAILURE;
            }
        }

        if (r->sem_name_1 != NULL) {
            if (sem_unlink(r->sem_name_1) == -1) {
                printf_errorchecked(stderr, "%s: Error in sem_unlink\n", r->argv[0]);
                exit_status = EXIT_FAILURE;
            }
        }

        if (r->sem_name_2 != NULL) {
            if (sem_unlink(r->sem_name_2) == -1) {
                printf_errorchecked(stderr, "%s: Error in sem_unlink\n", r->argv[0]);
                exit_status = EXIT_FAILURE;
            }
        }
    }

    exit(exit_status);
} // end remove_resources


