#include "sender_empfaenger.h"


int main(int argc, char **argv) {

    unsigned int length = arguments(argc, argv); // parst die commandline-argumente mit getopt
    g_length = length;

    make_names(); // generiert die Namen für die Semaphoren und den Shared-Memory

    sem_t * const sem_full  = sem_open_errorchecked(sem_name_1, O_CREAT, S_IRWXU, 0);
    g_sem_full = sem_full;
    sem_t * const sem_empty = sem_open_errorchecked(sem_name_2, O_CREAT, S_IRWXU, length);
    g_sem_empty = sem_empty;

    const int shared_memory = shm_open_errorchecked(shm_name_0, O_CREAT | O_RDWR, S_IRWXU);
    g_shared_memory = shared_memory;

    ftruncate_errorchecked(shared_memory, length * sizeof(int));

    int * const shared_mem_pointer = mmap_errorchecked(NULL, length * sizeof(int), PROT_WRITE, \
            MAP_SHARED, shared_memory, 0);
    g_shared_mem_pointer = shared_mem_pointer;
    
    int c = '\0'; // character für zeichenweises lesen/schreiben
    int i = 0;    // Index für shared-memory

    while (TRUE) {

        c = getchar(); 

        // critical section
        if (sem_wait(sem_empty) == -1) {
            perror_and_remove_resources("%s: Error in sem_wait\n", g_argv[0]);
        }

        *(shared_mem_pointer + i) = c; // writing in shared-memory

        if (sem_post(sem_full) == -1) {
            perror_and_remove_resources("%s: Error in sem_post\n", g_argv[0]);
        }
        // end critical section

        i++;
        i %= length; // weil Ringpuffer

        if (c == EOF) {
            break;
        }
    } // end while (TRUE)

    close_all(shared_memory, sem_full, sem_empty);

    if (munmap(shared_mem_pointer, length * sizeof(int)) == -1) {
        perror_and_remove_resources("%s: Error in munmap\n", g_argv[0]);
    }

    // kein sem_unlink(), da das nur ein Prozess machen sollte
    // kein shm_unlink(), da das nur ein Prozess machen sollte

    return EXIT_SUCCESS;
} // end main


