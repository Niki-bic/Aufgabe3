#include "sender_empfaenger_2.h"


int main(const int argc, const char * const * const argv) {
    struct resources r = {0}; // initial sind alle auf 0 bzw NULL

    init_resources(argc, argv, &r);                // initialisiert die Struktur 

    int c = '\0';                                  // Character für zeichenweises lesen/schreiben
    int i = 0;                                     // Index für shared-memory

    while (TRUE) {
        c = getchar();                             // Einlesen von stdin

        // critical section
        if (sem_wait(r.sem_empty) == -1) {         // down auf sem_empty
            printf_errorchecked(stderr, "%s: Error in sem_wait\n", r.argv[0]);
            remove_resources(EXIT_FAILURE, &r);
        }

        *(r.shared_mem_pointer + i) = c;           // writing in shared-memory
 
        if (sem_post(r.sem_full) == -1) {          // up auf sem_full
            printf_errorchecked(stderr, "%s: Error in sem_post\n", r.argv[0]);
            remove_resources(EXIT_FAILURE, &r);
        }
        // end critical section

        i++;
        i %= r.length;                             // weil Ringpuffer

        if (c == EOF) {
            break;
        }
    } // end while (TRUE)

    remove_resources(EXIT_SUCCESS, &r);            // Aufräumen
} // end main


