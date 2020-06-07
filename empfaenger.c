/**
 * @file empfaenger.c
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
 * \brief The main function to read from the shared memory ringbuffer.
 * @details This function initializes resources, calls sem_wait to 
 * block the critical section before entering, it reads from the 
 * shared memory ringbuffer and writes to stdout as long as 
 * the memory is full, it calls sem_post when leaving the critical 
 * section to unblock the critical section and indicate that 
 * it is free again.
 *
 * \param argc the number of arguments
 * \param argv command line arguments (including the program name in argv[0])
 *
 * \return None
 */
int main(const int argc, const char * const * const argv) {
    struct resources r = {0};                  
	 // initializes structure
    init_resources(argc, argv, &r);            
    // character for reading/writing
    int c = '\0';   
	// index for shared-memory	
    int i = 0;                                 

    while (TRUE) {      
		
		// locks semaphore
        sem_wait_errorchecked(r.sem_full, &r);  
		// reading from shared-memory
        c = *(r.shared_mem_pointer + i);        
		// unlocks semaphore
        sem_post_errorchecked(r.sem_empty, &r); 
       

        i++;
		// circular buffer
        i %= r.length;                          
		// checks if c EOF
        if (c == EOF) {
            break;
        }
		 // writing to stdout
        if (putchar(c) == EOF) {        
			// errorhandling		
            printf_errorchecked(stderr, "%s: Error in putchar\n", r.argv[0]);
			// cleaning up
            remove_resources(EXIT_FAILURE, &r);
        }
    }
	// cleaning up
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
