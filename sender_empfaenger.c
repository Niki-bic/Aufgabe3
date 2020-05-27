#include "sender_empfaenger.h"


unsigned int arguments(int argc, char **argv) {
    global_argv = argv;
    unsigned int length;
    int opt;
	
	if(argc != 3) { // check Eingabe
        perror_and_remove_resources("Usage: %s [-m] size\n", global_argv[0]);
    }

    while ((opt = getopt(argc, argv, ":m:")) != -1) {
        switch (opt) {
            case 'm':
                length = (unsigned int) string_to_int(optarg);
                break;
            default:
                perror_and_remove_resources("Usage: %s [-m] size\n", global_argv[0]);
                 
        }
    }
	
    if (optind != argc) { // check Eingabe
        perror_and_remove_resources("Usage: %s [-m] size\n", global_argv[0]);
    }

    return length;
} // end arguments


void make_names(void)  {
    const uid_t id = getuid();
    strcat(shm_name_0, "/shm_");
    strcat(sem_name_1, "/sem_");
    strcat(sem_name_2, "/sem_");
    (void) generate_name(shm_name_0, id, 0); // generates the name for shared-memory
    (void) generate_name(sem_name_1, id, 1); // generates the name for semaphore-full
    (void) generate_name(sem_name_2, id, 2); // generates the name for semaphore-empty
} // end make_name
    

char *generate_name(char *name, uid_t id, const int offset) {
    id *= 1000;
    id += offset;

    int num_digits = (int) log10((double) id) + 1;
    char *id_string = calloc(num_digits + 1, sizeof(char));

    int i = 0;
    while (id) {
        id_string[i] = id % 10 + 48;
        i++;
        id /= 10;
    }
    id_string[num_digits] = '\0';

    id_string = reverse_string(id_string);

    strcat(name, id_string);
    free(id_string);

    return name;
} // end generate_name


char *reverse_string(char *string) {
    int length = strlen(string);
    char temp;

    for (int i = 0; i < length / 2; i++) {
        temp = string[i];
        string[i] = string[length - 1 - i];
        string[length - 1 - i] = temp;
    }

    return string;
} // end reverse string


sem_t *sem_open_error_checked(const char *name, int oflag, mode_t mode, unsigned int value) {
    sem_t *sem_pointer = sem_open(name, oflag, mode, value);

    if (sem_pointer == SEM_FAILED) {
        perror_and_remove_resources("%s: Error in sem_open\n", global_argv[0]);
    }

    return sem_pointer;
} // end sem_open_error_checked


int shm_open_error_checked(const char *name, int oflag, mode_t mode) {
    int shared_memory = shm_open(name, oflag, mode);

    if (shared_memory == -1) {
        perror_and_remove_resources("%s: Error in shm_open\n", global_argv[0]);
         
    }

    return shared_memory;
} // end shm_open_error_checked


void ftruncate_error_checked(int fd, off_t length) {
    if (ftruncate(fd, length) == -1) {
        // print_errormessage("%s: Error in ftruncate\n", global_argv[0]);
         
    }
} // end ftruncate_error_checked


void *mmap_error_checked(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    void *shared_mem_pointer = mmap(addr, length, prot, flags, fd, offset);
    
    if (shared_mem_pointer == MAP_FAILED) {
        perror_and_remove_resources("%s: Error in mmap\n", global_argv[0]);
         
    }

    return shared_mem_pointer;
} // end mmap_error_checked


void close_all(int shared_memory, sem_t *sem_full, sem_t *sem_empty) {
    if (close(shared_memory) == -1) {
        perror_and_remove_resources("%s: Error in close\n", global_argv[0]);
    }

    if (sem_close(sem_full) == -1) {
        perror_and_remove_resources("%s: Error in sem_close\n", global_argv[0]);
    }

    if (sem_close(sem_empty) == -1) {
        perror_and_remove_resources("%s: Error in sem_close\n", global_argv[0]);
    }

     
} // end close_all


void unlink_all_sem(char *sem_name_1, char *sem_name_2) {
    if (sem_unlink(sem_name_1) == -1) {
        perror_and_remove_resources("%s: Error in sem_unlink\n", global_argv[0]);
    }

    if (sem_unlink(sem_name_2) == -1) {
        perror_and_remove_resources("%s: Error in sem_unlink\n", global_argv[0]);
    }

     
} // end unlink_sem


int string_to_int(const char * const string){
	char *end_ptr;
	errno = 0;
	long number = strtol(string, &end_ptr, 10);
	if(*end_ptr != '\0' || errno != 0){
        perror_and_remove_resources("%s: Error in strtol\n", global_argv[0]);
         
	}

	return (int) number;
} // end string_to_int


void print_errormessage(const char * const string, ...){
	va_list array;
	va_start(array, string);

	vfprintf(stderr, string, array); 

	va_end(array);
} // end print_errormessage


void perror_and_remove_resources(const char * const string, ...){
	va_list array;
	va_start(array, string);

	vfprintf(stderr, string, array); 

	va_end(array);

    if (g_sem_full != NULL) {
        if (sem_close(g_sem_full) == -1) {
            print_errormessage("%s: Error in sem_close\n", global_argv[0]);
        }
    }

    if (g_sem_empty != NULL) {
        if (sem_close(g_sem_empty) == -1) {
            print_errormessage("%s: Error in sem_close\n", global_argv[0]);
        }
    }

    if (g_shared_mem_pointer != NULL) {
        if (munmap(g_shared_mem_pointer, g_length * sizeof(int)) == -1) {
            print_errormessage("%s: Error in munmap\n", global_argv[0]);
        }
    }

    if (g_shared_memory != 0) {
        if (close(g_shared_memory) == -1) {
            print_errormessage("%s: Error in close\n", global_argv[0]);
        }
    }

    if (shm_name_0 != NULL) {
        if (shm_unlink(shm_name_0) == -1) {
            print_errormessage("%s: Error in shm_unlink\n", global_argv[0]);
        }
    }

    if (sem_name_1 != NULL) {
        if (sem_unlink(sem_name_1) == -1) {
            print_errormessage("%s: Error in sem_unlink\n", global_argv[0]);
        }
    }

    if (sem_name_2 != NULL) {
        if (sem_unlink(sem_name_2) == -1) {
            print_errormessage("%s: Error in sem_unlink\n", global_argv[0]);
        }
    }

    exit(EXIT_FAILURE);
} // end perror_and_remove_resources


