#include "sender_empfaenger.h"


int main(int argc, char **argv) {

    // if (argc != 2) {
    //     printf("Usage:\n");
    //     return EXIT_FAILURE;
    // }

    // parst die commandline-arguemente mit getopt
    unsigned int length = arguments(argc, argv); // error-checking einbauen

    make_names(); // generiert die Namen für die Semaphoren und den Shared-Memory

    sem_t * const sem_mutex = sem_open_error_checked(sem_name_1, O_CREAT, S_IRWXU, 1); // eventuell nicht notwendig
    sem_t * const sem_full  = sem_open_error_checked(sem_name_2, O_CREAT, S_IRWXU, 0);
    sem_t * const sem_empty = sem_open_error_checked(sem_name_3, O_CREAT, S_IRWXU, length);

    errno = 0;
    const int shared_memory = shm_open(shm_name_0, O_CREAT | O_RDONLY, S_IRWXU); // error-checking noch einbauen

    if (ftruncate(shared_memory, length * sizeof(int)) == -1) {
        // error
    }
   
    int * const shared_mem_pointer = mmap(NULL, length * sizeof(int), PROT_READ, MAP_SHARED, shared_memory, 0);
    if (shared_mem_pointer == (int * const) MAP_FAILED) {
        fprintf(stderr, "mmap failed in %s\n", argv[0]);
        // error
    }
    
    int c = '\0'; // char für zeichenweises lesen/schreiben
    int i = 0;

    while (TRUE) {      
        (void) sem_wait(sem_full); // return value -1 und errno == EINTR prüfen

        // critical section
        (void) sem_wait(sem_mutex); // same
        c = *(shared_mem_pointer + i);
        (void) sem_post(sem_mutex); // error-checking noch einbauen
        // end critical section

        i++;
        i %= length;

        putchar(c);
        (void) sem_post(sem_empty); // same
        
        if (c == EOF) {
            break;
        }
    }

    close_all(shared_memory, sem_mutex, sem_full, sem_empty);

    unlink_sem(sem_name_1, sem_name_2, sem_name_3);

    if (munmap(shared_mem_pointer, length * sizeof(int)) == -1) {
        // error
    }

    if (shm_unlink(shm_name_0) == -1) {
        // error
    }

    return EXIT_SUCCESS;
} // end main


