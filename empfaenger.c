#include "sender_empfaenger.h"


int main(int argc, char **argv) {

    unsigned int length = arguments(argc, argv); // parst die commandline-argumente mit getopt
    g_length = length;

    make_names(); // generiert die Namen für die Semaphoren und den Shared-Memory

    sem_t * const sem_full  = sem_open_errorchecked(sem_name_1, O_CREAT, S_IRWXU, 0);
    g_sem_full = sem_full;
    sem_t * const sem_empty = sem_open_errorchecked(sem_name_2, O_CREAT, S_IRWXU, length);
    g_sem_empty = sem_empty;

    const int shared_memory = shm_open_errorchecked(shm_name_0, O_CREAT | O_RDONLY, S_IRWXU);
    g_shared_memory = shared_memory;

    ftruncate_errorchecked(shared_memory, length * sizeof(int));
   
    int * const shared_mem_pointer = mmap_errorchecked(NULL, length * sizeof(int), PROT_READ, \
            MAP_SHARED, shared_memory, 0);
    g_shared_mem_pointer = shared_mem_pointer;

    
    int c = '\0'; // char für zeichenweises lesen/schreiben
    int i = 0;    // Index für shared-memory

    while (TRUE) {      

        // critical section
        errno = 0;
        if (sem_wait(sem_full) == -1) {
            // error
        }

        c = *(shared_mem_pointer + i); // reading from shared-memory

        errno = 0;
        if (sem_post(sem_empty) == -1) {
            // error
        }
        // end critical section

        i++;
        i %= length;

        if (c == EOF) {
            break;
        }

        putchar(c);
    } // end while (TRUE)

    close_all(shared_memory, sem_full, sem_empty);

    if (munmap(shared_mem_pointer, length * sizeof(int)) == -1) {
        perror_and_remove_resources("%s: Error in munmap\n", global_argv[0]);
    }

    unlink_all_sem(sem_name_1, sem_name_2);

    if (shm_unlink(shm_name_0) == -1) {
        perror_and_remove_resources("%s: Error in shm_unlink\n", global_argv[0]);
    }

    return EXIT_SUCCESS;
} // end main


