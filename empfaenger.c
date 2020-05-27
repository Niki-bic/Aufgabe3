#include "sender_empfaenger.h"


int main(int argc, char **argv) {

    unsigned int length = arguments(argc, argv); // parst die commandline-arguemente mit getopt

    make_names(); // generiert die Namen für die Semaphoren und den Shared-Memory

    sem_t * const sem_full  = sem_open_error_checked(sem_name_1, O_CREAT, S_IRWXU, 0);
    sem_t * const sem_empty = sem_open_error_checked(sem_name_2, O_CREAT, S_IRWXU, length);

    const int shared_memory = shm_open_error_checked(shm_name_0, O_CREAT | O_RDONLY, S_IRWXU);

    ftruncate_error_checked(shared_memory, length * sizeof(int));
   
    int * const shared_mem_pointer = mmap_error_checked(NULL, length * sizeof(int), PROT_READ, \
            MAP_SHARED, shared_memory, 0);
    
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

    unlink_all_sem(sem_name_1, sem_name_2);

    if (munmap(shared_mem_pointer, length * sizeof(int)) == -1) {
        // error
    }

    if (shm_unlink(shm_name_0) == -1) {
        // error
    }

    return EXIT_SUCCESS;
} // end main


