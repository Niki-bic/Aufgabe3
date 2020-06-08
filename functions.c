/**
 * @file functions.c
 * BES - sender/empfaenger
 * Projekt 3
 *
 * Gruppe 13
 *
 * @author Binder Patrik         <ic19b030@technikum-wien.at>
 * @author Ferchenbauer Nikolaus <ic19b013@technikum-wien.at>
 * @author Pittner Stefan        <ic19b003@technikum-wien.at>
 * @date 2020/06/07
 *
 * @version 1.x
 *
 * @todo nothing
 *
 */

/**
 * -------------------------------------------------------------- includes --
 */

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

/**
 * ------------------------------------------------------------- functions --
 */

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

/**
 * \brief Initiate needed resources.
 * @details This function initializes all needed variables.
 *
 * \param argc 
 * \param argv
 * \param r struct with bundled parameters
 *
 * \return None
 */
// initializes all members of structure r
void init_resources(const int argc, const char * const * const argv, struct resources * const r) {
    r->argc = (int) argc;
    r->argv = (char * const *) argv;
	 // check arguments and initialize r->length
    check_arguments(r);                              
	 // create name for shared-memory
	create_name(r->shm_name_0, 0, "/shm_", r);   
	// create name for first semaphore
    create_name(r->sem_name_1, 1, "/sem_", r);
	// create name for second semaphore
    create_name(r->sem_name_2, 2, "/sem_", r);       

    // create semaphore full
    r->sem_full  = sem_open_errorchecked(r->sem_name_1, O_CREAT, S_IRWXU, 0, r);
    // create semaphore empty
    r->sem_empty = sem_open_errorchecked(r->sem_name_2, O_CREAT, S_IRWXU, r->length, r);

    // check if sender or empfaenger and create shared memory
    if (strcmp(argv[0], "./sender") == 0) {
        r->shared_memory = shm_open_errorchecked(r->shm_name_0, O_CREAT | O_RDWR, S_IRWXU, r);
		// sets the size of the file referenced by the filedescriptor fd to length bytes
        ftruncate_errorchecked(r->shared_memory, r->length * sizeof(int), r);
		// mapping shared memory
        r->shared_mem_pointer = mmap_errorchecked(NULL, r->length * sizeof(int), \
                PROT_WRITE, MAP_SHARED, r->shared_memory, 0, r);
    } else {     // strcmp(argv[0], "./empfaenger") == 0
		//create shared memory
        r->shared_memory = shm_open_errorchecked(r->shm_name_0, O_CREAT | O_RDONLY, S_IRWXU, r);
		// sets the size of the file referenced by the filedescriptor fd to length bytes (setting the size of the shared-memory)
        ftruncate_errorchecked(r->shared_memory, r->length * sizeof(int), r);
		// mapping shared memory
        r->shared_mem_pointer = mmap_errorchecked(NULL, r->length * sizeof(int), \
                PROT_READ, MAP_SHARED, r->shared_memory, 0, r);
    }
} // end init_resources

/**
 * \brief Check input arguments.
 * @details This function checks the arguments passed 
 * on when the main function is called and determines
 * the length of the shared memory.
 *
 * \param r struct with bundled parameters
 *
 * \return None
 */
// check the commandline arguments and initialize r->length
static void check_arguments(struct resources * const r) {
    int opt;
	// check if correct numbers of argc
	if(r->argc != 2 && r->argc != 3) {       
		// errorhandling
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }

    // get option i.e. the length (= size) of the circular buffer
    while ((opt = getopt(r->argc, r->argv, ":m:")) != -1) {
        switch (opt) {
            case 'm':
				// write the errorchecked size of the circular buffer in r->length
                r->length = strtol_errorchecked(optarg, r);
                break;
            default:
				// errorhandling
                printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
				// clean up function
                remove_resources(EXIT_FAILURE, r);
        }
    }
	// check input again
    if (optind != r->argc) {   
		// errorhandling
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }
}

/**
 * \brief Check memory space input length with strtol and length of memory.
 * @details This function checks if the input 
 * parameters are valid. It checks either if it is a number,
 * if it is out of range, if it ist too big, if it has not a 
 * valid size or if characters are found in the string
 * instead of numbers.
 * 
 * \param string optarg argument
 * \param r struct with bundled parameters
 *
 * \return value of strtol
 * \retval number
 */
// convert string to unsigned long and checks for errors
static unsigned long strtol_errorchecked(const char * const string, struct resources * const r){
	char *end_ptr;
	errno = 0;
	unsigned long number = (unsigned long) strtol(string, &end_ptr, 10);
	// checks if the input is a number
    if (end_ptr == string) {
		// errorhandling
        printf_errorchecked(stderr, "Usage: %s not an integral number\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
	// check if all characters were converted
    } else if (*end_ptr != '\0') {
		// errorhandling
        printf_errorchecked(stderr, "Usage: %s extra characters at end of input\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
	// checks if the number is out of range
    } else if (number == ULONG_MAX && errno == ERANGE) {
		// errorhandling
        printf_errorchecked(stderr, "Usage: %s number out of range\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
	// checks if the number are too big (max unsigned long)
    } else if (number > ULONG_MAX) {
		// errorhandling
        printf_errorchecked(stderr, "Usage: %s number too big\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
	// check if number is smaller than zero or if higher than maximum size of a single shared memory
    } else if (number <= 0 || number >= SHM_MAX) {
		// errorhandling
        printf_errorchecked(stderr, "Usage: %s: not a valid size\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }

	return number;
} 

/**
 * \brief Create name for semaphores.
 * @details This function creates the name for the semaphores.
 * 
 * \param name variable to initialize from struct
 * \param offset value specified by the guideline
 * \param prefix name specified
 * \param r struct with bundled parameters
 *
 * \return None
 */
// create name for the semaphore and shared memory
static void create_name(char *name, const unsigned int offset, const char * const prefix, \
        struct resources * const r) {
	// create name for shared memory or shared memory
    strncpy(r->shm_name_0, prefix, PREFIX_LEN);             
	// getuid number
	uid_t user_id = getuid() * 1000 + offset;                
	char uid_string[UID_LEN];
	// check return value of snprintf
	if (snprintf(uid_string, UID_LEN, "%d", user_id) < 0) {
		// errorhandling
        printf_errorchecked(stderr, "Usage: %s -m size\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }
    // create name for semaphore or shared memory
	strncat(name, uid_string, UID_LEN);        
	// terminating the string with '\0'
    name[PREFIX_LEN + UID_LEN] = '\0';
} 
    
/**
 * \brief Creates new or opens existing semaphore.
 * @details This functions is an error-checking wrapper for the system call sem_open.
 * All resources are removed in case of an error.
 * 
 * \param name identifies the semaphore
 * \param oflag specifies flags
 * \param mode specifies permissions
 * \param value initial value for new semaphore
 * \param r struct with bundled parameters
 *
 * \return value of strtol
 * \retval sem_pointer
 */
// create or open semaphore
static sem_t *sem_open_errorchecked(const char * const name, const int oflag, \
        const mode_t mode, const unsigned int value, struct resources * const r) {
	// open, create semaphore
    sem_t *sem_pointer = sem_open(name, oflag, mode, value);
	// check if open(create) semaphore failed
    if (sem_pointer == SEM_FAILED) {
        printf_errorchecked(stderr, "%s: Error in sem_open\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }

    return sem_pointer;
}

/**
 * \brief Create shared memory if needed.
 * @details This function creates and opens a new, 
 * or opens an existing, POSIX shared memory object.
 * 
 * \param name specifies shared memory object
 * \param oflag bit mask specified access mode
 * \param mode specifies permissions
 * \param r struct with bundled parameters
 *
 * \return returnvalue of shm_open 
 * \retval shared_memory
 */
// create or open shared-memory
static int shm_open_errorchecked(const char * const name, const int oflag, \
        const mode_t mode, struct resources * const r) {
	// open, create shared memory
    int shared_memory = shm_open(name, oflag, mode);
	// check if open(create) shared memory failed
    if (shared_memory == -1) {
		// errorhandling
        printf_errorchecked(stderr, "%s: Error in shm_open\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }

    return shared_memory;
} 

/**
 * \brief Check ftruncate.
 * @details This function is called by init_resources, it checks
 * ftruncate for errors and removes all resources in case of an error.
 *
 * \param fd file descriptor from shm_open
 * \param length shared memory size
 * \param r struct with bundled parameters
 *
 * \return None
 */
// setting the size of the shared-memory
static void ftruncate_errorchecked(int fd, const off_t length, struct resources * const r) {
	//set errno to zero
    errno = 0;
	// check if ftruncate returns an error
    if (ftruncate(fd, length) == -1 && errno != 0 && errno != EINVAL) {
		// errorhandling
        printf_errorchecked(stderr, "%s: Error in ftruncate\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }
}

/**
 * \brief Check mmap for errors.
 * @details This function is called by init_resources, it
 * checks mmap for errors and removes all resources in case of an error.
 *
 * \param addr virtual address space of the calling process
 * \param length shared memory mapping length
 * \param prot desired memory protection
 * \param flags determines whether updates to the mapping are visible
 * \param fd file descriptor of th shared memory
 * \param offset starting point in the file referred to by the file descriptor
 * \param r struct with bundled parameters
 *
 * \return returnvalue of mmap
 * \retval shared_mem_pointer 
 */
// map the shared-memory into the virtual-memory
static int *mmap_errorchecked(void *addr, const size_t length, const int prot, \
        const int flags, const int fd, const off_t offset, struct resources * const r) {
    void *shared_mem_pointer = mmap(addr, length, prot, flags, fd, offset);
    // check if mapping shared memory failed
    if (shared_mem_pointer == MAP_FAILED) {
		// errorhandling
        printf_errorchecked(stderr, "%s: Error in mmap\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }

    return shared_mem_pointer;
}

/**
 * \brief Check printf for errors.
 * @details This function checks printf for errors.
 * 
 * \param stream pointer to a file stream
 * \param string output string
 * \param ... variable argument
 *
 * \return None
 */
// an error-checking wrapper for fprintf
void printf_errorchecked(FILE * const stream, const char * const string, ...) {
	// va is used to generate a function with arguments that can vary
	va_list array;
	va_start(array, string);
	
	if (vfprintf(stream, string, array) < 0) {
		// errorhandling
		fprintf(stderr, "Error in vfprintf()!\n");
		exit(EXIT_FAILURE);
	}

	va_end(array);
}

/**
 * \brief Close and remove used resources.
 * @details This function removes used resources
 * in case of an error or if the program terminates normally.
 * 
 * \param exit_status returned exit paramter 
 * \param r struct with bundled parameters
 *
 * \return exit 
 * \retval exit_status
 */
// cleaning up on failure or at the end
void remove_resources(int exit_status, struct resources * const r) {
	// check if sem_full exists
    if (r->sem_full != NULL) {
		// check if error while closing semaphore
        if (sem_close(r->sem_full) == -1) {
			// errorhandling
            printf_errorchecked(stderr, "%s: Error in sem_close\n", r->argv[0]);
            exit_status = EXIT_FAILURE;
        }
    }
	// check if sem_empty exists
    if (r->sem_empty != NULL) {
		// check if error while closing semaphore
        if (sem_close(r->sem_empty) == -1) {
			// errorhandling
            printf_errorchecked(stderr, "%s: Error in sem_close\n", r->argv[0]);
            exit_status = EXIT_FAILURE;
        }
    }
	// check if shared_memory exists
    if (r->shared_memory != 0) {
		// check if error while closing shared memory
        if (close(r->shared_memory) == -1) {
			// errorhandling
            printf_errorchecked(stderr, "%s: Error in close\n", r->argv[0]);
            exit_status = EXIT_FAILURE;
        }
    }
	// check if virtual-memory exists
    if (r->shared_mem_pointer != NULL) {
		// check if error while unmapping shared memory
        if (munmap(r->shared_mem_pointer, r->length * sizeof(int)) == -1) {
			// errorhandling
            printf_errorchecked(stderr, "%s: Error in munmap\n", r->argv[0]);
            exit_status = EXIT_FAILURE;
        }
    }


    // only in ./empfaenger or in case of failure should this be done
    // only one of both processes should unlink in case of normal termination: the ./empfaenger
    if (strcmp(r->argv[0], "./empfaenger") == 0 || exit_status == EXIT_FAILURE) {
        // check if shared memory name exists
		if (r->shm_name_0 != NULL) {
			// check if error removing shared memory object name
            if (shm_unlink(r->shm_name_0) == -1) {
				// errorhandling
                printf_errorchecked(stderr, "%s: Error in shm_unlink\n", r->argv[0]);
                exit_status = EXIT_FAILURE;
            }
        }
		// check if named semaphore exists
        if (r->sem_name_1 != NULL) {
			// check if error removing named semaphore
            if (sem_unlink(r->sem_name_1) == -1) {
				// errorhandling
                printf_errorchecked(stderr, "%s: Error in sem_unlink\n", r->argv[0]);
                exit_status = EXIT_FAILURE;
            }
        }
		// check if named semaphore exists
        if (r->sem_name_2 != NULL) {
			// check if error removing named semaphore
            if (sem_unlink(r->sem_name_2) == -1) {
				// errorhandling
                printf_errorchecked(stderr, "%s: Error in sem_unlink\n", r->argv[0]);
                exit_status = EXIT_FAILURE;
            }
        }
    }

    exit(exit_status);
} 

/**
 * \brief Check sem_wait for errors.
 * @details This function is called from the main function, it
 * checks sem_wait for errors and removes all resources in case of an error.
 * sem_wait() decrements (locks) the semaphore pointed to by sem.
 *
 * \param sem semaphore pointed to from sem_open
 * \param r struct with bundled parameters
 *
 * \return None
 */
// down on semaphore
void sem_wait_errorchecked(sem_t * const sem, struct resources * const r) {
	// check if error locking semaphore
    if (sem_wait(sem) == -1) {
		// errorhandling
        printf_errorchecked(stderr, "%s: Error in sem_wait\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }
}

/**
 * \brief Check sem_post for errors.
 * @details This function is called from the main function, it
 * checks sem_post for errors and removes all resources in case of an error.
 * sem_post() increments (unlocks) the semaphore pointed to by sem.
 *
 * \param sem semaphore pointed to from sem_open
 * \param r struct with bundled parameters
 *
 * \return None
 */
// up on semaphore
void sem_post_errorchecked(sem_t * const sem, struct resources * const r) {
	// check if error unlocking semaphore
    if (sem_post(sem) == -1) {
		// errorhandling
        printf_errorchecked(stderr, "%s: Error in sem_post\n", r->argv[0]);
		// clean up function
        remove_resources(EXIT_FAILURE, r);
    }
} 

/**
 * =================================================================== eof ==
 */

 /**
 * mode: c
 * c-mode: k&r
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * end
 *
 */
