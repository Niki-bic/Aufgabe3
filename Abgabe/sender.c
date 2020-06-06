#include "sender_empfaenger.h"


int main(const int argc, const char * const * const argv) {
    struct resources r = {0};                      // setting all members to 0 

    init_resources(argc, argv, &r);                // initializes the structure

    int c = '\0';                                  // character for reading/writing
    int i = 0;                                     // index for shared-memory

    while (TRUE) {
        c = getchar();                             // reading from stdin
        if (c == EOF && ferror(stdin)) {
            printf_errorchecked(stderr, "%s: Error in getchar\n", r.argv[0]);
            remove_resources(EXIT_FAILURE, &r);
        }

        // critical section
        sem_wait_errorchecked(r.sem_empty, &r);    // down auf sem_empty

        *(r.shared_mem_pointer + i) = c;           // writing in shared-memory
 
        sem_post_errorchecked(r.sem_full, &r);     // up auf sem_full
        // end critical section

        i++;
        i %= r.length;                             // circular buffer 

        if (c == EOF) {
            break;
        }
    } // end while (TRUE)

    remove_resources(EXIT_SUCCESS, &r);            // cleaning up
} // end main
