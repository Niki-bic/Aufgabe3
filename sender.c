/**
 * @file sender.c
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

/**
 * ------------------------------------------------------------- functions --
 */

/**
 *
 * \brief The main function to write to the shared memory ringbuffer.
 * @details This function initializes resources, calls sem_wait to 
 * block the critical section before entering, it writes to the 
 * shared memory ringbuffer for the given length given to it, 
 * it then calls sem_post when leaving the critical section 
 * to unblock the critical section and indicate that it is
 * free again.
 *
 * \param argc the number of arguments
 * \param argv command line arguments (including the program name in argv[0])
 *
 * \return None
 */
int main(const int argc, const char * const * const argv) {
	// setting all members to 0 
    struct resources r = {0};                      
	// initializes the structure
    init_resources(argc, argv, &r);                
	// character for reading/writing
    int c = '\0';    
	// index for shared-memory	
    int i = 0;                                   

    while (TRUE) {
        c = getchar();
		 // reading from stdin
        if (c == EOF && ferror(stdin)) {
			// errorhandling
            printf_errorchecked(stderr, "%s: Error in getchar\n", r.argv[0]);
			// clean up function
            remove_resources(EXIT_FAILURE, &r);
        }

		// locks semaphore
        sem_wait_errorchecked(r.sem_empty, &r);    
		// writing in shared-memory
        *(r.shared_mem_pointer + i) = c;           
		// unlocks semaphore
        sem_post_errorchecked(r.sem_full, &r);     
       
		// circular buffer 
        i++;
        i %= r.length;                             
		// checks if c EOF
        if (c == EOF) {
            break;
        }
    }
	// clean up function
    remove_resources(EXIT_SUCCESS, &r);           
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
