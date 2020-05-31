#include "sender_empfaenger.h"

#define TRUE 1


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
