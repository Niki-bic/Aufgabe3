#include "sender_empfaenger.h"


#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#define PREFIX_LEN 5
#define SHM_MAX 2147483648
#define UID_LEN 8 


static void check_arguments(struct resources * const r);
static unsigned long strtol_errorchecked(const char * const string, struct resources * const r);
static void create_name(char *name, const unsigned int offset, const char * const prefix, \
        struct resources * const r);
static sem_t *sem_open_errorchecked(const char * const name, const int oflag, \
        const mode_t mode, const unsigned int value, struct resources * const r);
static int shm_open_errorchecked(const char * const name, const int oflag, \
        const mode_t mode, struct resources * const r);
static void ftruncate_errorchecked(int fd, const off_t length, struct resources * const r);
static int *mmap_errorchecked(void *addr, const size_t length, const int prot, \
        const int flags, const int fd, const off_t offset, struct resources * const r);


// initializes all members of the structure r
void init_resources(const int argc, const char * const * const argv, struct resources * const r) {
    r->argc = (int) argc;
    r->argv = (char * const *) argv;

    check_arguments(r);                               // check arguments and initialize r->length

	create_name(r->shm_name_0, 0, "/shm_", r);        // create name for shared-memory
    create_name(r->sem_name_1, 1, "/sem_", r);        // create name for first semaphore
    create_name(r->sem_name_2, 2, "/sem_", r);        // create name for second semaphore

    // create or open semaphore full
    r->sem_full  = sem_open_errorchecked(r->sem_name_1, O_CREAT, S_IRWXU, 0, r);
    // create or open semaphore empty
    r->sem_empty = sem_open_errorchecked(r->sem_name_2, O_CREAT, S_IRWXU, r->length, r);

    // check if sender or empfaenger and create shared memory
    if (strcmp(argv[0], "./sender") == 0) {
        r->shared_memory = shm_open_errorchecked(r->shm_name_0, O_CREAT | O_RDWR, S_IRWXU, r);
        ftruncate_errorchecked(r->shared_memory, r->length * sizeof(int), r);
        r->shared_mem_pointer = mmap_errorchecked(NULL, r->length * sizeof(int), \
                PROT_WRITE, MAP_SHARED, r->shared_memory, 0, r);
    } else {     // strcmp(argv[0], "./empfaenger") == 0
        r->shared_memory = shm_open_errorchecked(r->shm_name_0, O_CREAT | O_RDONLY, S_IRWXU, r);
        ftruncate_errorchecked(r->shared_memory, r->length * sizeof(int), r);
            r->shared_mem_pointer = mmap_errorchecked(NULL, r->length * sizeof(int), \
                PROT_READ, MAP_SHARED, r->shared_memory, 0, r);
    }
} // end init_resources


// check the commanline arguments and initialize r.length
static void check_arguments(struct resources * const r) {
    int opt;
	
	if(r->argc != 2 && r->argc != 3) {                           // check input
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

    // get option i.e. the length (= size) of the circular buffer
    while ((opt = getopt(r->argc, r->argv, ":m:")) != -1) {
        switch (opt) {
            case 'm':
                r->length = strtol_errorchecked(optarg, r);
                break;
            default:
                printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
                remove_resources(EXIT_FAILURE, r);
        }
    }
	
    if (optind != r->argc) {                                     // check input again
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }
} // end check_arguments


// convert the string to unsigned long
static unsigned long strtol_errorchecked(const char * const string, struct resources * const r){
	char *end_ptr;
	errno = 0;
	unsigned long number = (unsigned long) strtol(string, &end_ptr, 10);

    if (end_ptr == string) {
        printf_errorchecked(stderr, "Usage: %s not an integral number\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    } else if (*end_ptr != '\0') {
        printf_errorchecked(stderr, "Usage: %s extra characters at end of input\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    } else if (number == ULONG_MAX && errno == ERANGE) {
        printf_errorchecked(stderr, "Usage: %s number out of range\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    } else if (number > ULONG_MAX) {
        printf_errorchecked(stderr, "Usage: %s number too big\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    } else if (number <= 0 || number >= SHM_MAX) {
        printf_errorchecked(stderr, "Usage: %s: not a valid size\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

	return number;
} // end strtol_errorchecked


// create the names for the semaphore and shared memory
static void create_name(char *name, const unsigned int offset, const char * const prefix, \
        struct resources * const r) {
    strncpy(r->shm_name_0, prefix, PREFIX_LEN);              // no sane error-checking in strncpy possible

	uid_t user_id = getuid() * 1000 + offset;                // getuit() is always successfull
	char uid_string[UID_LEN];

	if (snprintf(uid_string, UID_LEN, "%d", user_id) < 0) {
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }
        
	strncat(name, uid_string, UID_LEN);                     // no sane error-checking in strncat possible
    name[PREFIX_LEN + UID_LEN] = '\0';
} // end create_name
    

// create or open semaphore
static sem_t *sem_open_errorchecked(const char * const name, const int oflag, \
        const mode_t mode, const unsigned int value, struct resources * const r) {
    sem_t *sem_pointer = sem_open(name, oflag, mode, value);

    if (sem_pointer == SEM_FAILED) {
        printf_errorchecked(stderr, "%s: Error in sem_open\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

    return sem_pointer;
} // end sem_open_errorchecked


// create or open shared-memory
static int shm_open_errorchecked(const char * const name, const int oflag, \
        const mode_t mode, struct resources * const r) {
    int shared_memory = shm_open(name, oflag, mode);

    if (shared_memory == -1) {
        printf_errorchecked(stderr, "%s: Error in shm_open\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

    return shared_memory;
} // end shm_open_errorchecked


// setting the size of the shared-memory
static void ftruncate_errorchecked(int fd, const off_t length, struct resources * const r) {
    errno = 0;

    if (ftruncate(fd, length) == -1 && errno != 0 && errno != EINVAL) {
        printf_errorchecked(stderr, "%s: Error in ftruncate\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }
} // end ftruncate_errorchecked


// map the shared-memory into the virtual-memory
static int *mmap_errorchecked(void *addr, const size_t length, const int prot, \
        const int flags, const int fd, const off_t offset, struct resources * const r) {
    void *shared_mem_pointer = mmap(addr, length, prot, flags, fd, offset);
    
    if (shared_mem_pointer == MAP_FAILED) {
        printf_errorchecked(stderr, "%s: Error in mmap\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }

    return shared_mem_pointer;
} // end mmap_errorchecked


// an error-checking wrapper for fprintf
void printf_errorchecked(FILE * const stream, const char * const string, ...) {
	va_list array;
	va_start(array, string);

	if (vfprintf(stream, string, array) < 0) {
		fprintf(stderr, "Error in vfprintf()!\n");
		exit(EXIT_FAILURE);
	}

	va_end(array);
} // end printf_errorchecked


// cleaning up after failure or at the end
void remove_resources(int exit_status, struct resources * const r) {
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


    // only in ./empfaenger or in case of failure should this be done
    // only one of both processes should unlink in case of normal termination: the ./empfaenger
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


// down on semaphore
void sem_wait_errorchecked(sem_t * const sem, struct resources * const r) {
    if (sem_wait(sem) == -1) {
        printf_errorchecked(stderr, "%s: Error in sem_wait\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }
} // end sem_wait_errorchecked


// up on semaphore
void sem_post_errorchecked(sem_t * const sem, struct resources * const r) {
    if (sem_post(sem) == -1) {
        printf_errorchecked(stderr, "%s: Error in sem_post\n", r->argv[0]);
        remove_resources(EXIT_FAILURE, r);
    }
} // end sem_post_errorchecked

