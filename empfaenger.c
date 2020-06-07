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
 * \brief The main fuction to read from the shared memory ringbuffer.
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
    struct resources r = {0};                   // setting all members to 0

    init_resources(argc, argv, &r);             // initializes structure
    
    int c = '\0';                               // character for reading/writing
    int i = 0;                                  // index for shared-memory

    while (TRUE) {      

        // critical section
        sem_wait_errorchecked(r.sem_full, &r);  // down on sem_full

        c = *(r.shared_mem_pointer + i);        // reading from shared-memory

        sem_post_errorchecked(r.sem_empty, &r); // up on sem_empty
        // end critical section

        i++;
        i %= r.length;                          // circular buffer

        if (c == EOF) {
            break;
        }

        if (putchar(c) == EOF) {                // writing to stdout
            printf_errorchecked(stderr, "%s: Error in putchar\n", r.argv[0]);
            remove_resources(EXIT_FAILURE, &r);
        }
    } // end while (TRUE)

    remove_resources(EXIT_SUCCESS, &r);         // cleaning up
} // end main

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