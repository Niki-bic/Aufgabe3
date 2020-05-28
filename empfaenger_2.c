#include "sender_empfaenger_2.h"


int main(int argc, char **argv) {

    struct resources r = {0}; // initial sind alle auf 0 bzw NULL

    init_resources(&r, argc, argv);
    
    int c = '\0'; // character für zeichenweises lesen/schreiben
    int i = 0;    // Index für shared-memory

    while (TRUE) {      

        // critical section
        if (sem_wait(r.sem_full) == -1) {
            printf_errorchecked(stderr, "%s: Error in sem_wait\n", r.argv[0]);
            remove_resources(&r, EXIT_FAILURE);
        }

        c = *(r.shared_mem_pointer + i); // reading from shared-memory

        if (sem_post(r.sem_empty) == -1) {
            printf_errorchecked(stderr, "%s: Error in sem_post\n", r.argv[0]);
            remove_resources(&r, EXIT_FAILURE);
        }
        // end critical section

        i++;
        i %= r.length; // weil Ringpuffer

        if (c == EOF) {
            break;
        }

        if (putchar(c) == EOF) {
            printf_errorchecked(stderr, "%s: Error in putchar\n", r.argv[0]);
            remove_resources(&r, EXIT_FAILURE);
        }
    } // end while (TRUE)

    remove_resources(&r, EXIT_SUCCESS);

    return EXIT_SUCCESS;
} // end main


