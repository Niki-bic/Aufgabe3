#include "sender_empfaenger.h"


int main(int argc, char **argv) {

    // if (argc != 2) {
    //     printf("Usage:\n");
    //     return EXIT_FAILURE;
    // }

    int opt;
    int length;

    while ((opt = getopt(argc, argv, "m:")) != -1) {
        switch (opt) {
            case 'm':
                length = (unsigned int) strtol(optarg, 0, 10);
                // printf("%d\n", length);
                break;
            case '?':
                // error: unknown option
                break;
            default:

                break;
        }
    }

    const uid_t id = getuid();
    (void) generate_name(shm_name_0, id, 0);
    (void) generate_name(sem_name_1, id, 1);
    (void) generate_name(sem_name_2, id, 2);
    (void) generate_name(sem_name_3, id, 3);

    sem_t * const sem_mutex = sem_open(sem_name_1, O_CREAT, S_IRWXU, 1);
    if (sem_mutex == SEM_FAILED) {
        fprintf(stderr, "sem_mutex failed in sender\n");
        // error in sem_open()
    }
    sem_t * const sem_full = sem_open(sem_name_2, O_CREAT, S_IRWXU, 0);
    if (sem_full == SEM_FAILED) {
        fprintf(stderr, "sem_full failed in sender\n");
        // error in sem_open()
    }
    sem_t * const sem_empty = sem_open(sem_name_3, O_CREAT, S_IRWXU, length); // vl length - 1
    if (sem_empty == SEM_FAILED) {
        fprintf(stderr, "sem_empty failed in sender\n");
        // error in sem_open()
    }

    errno = 0;
    const int shared_memory = shm_open(shm_name_0, O_CREAT | O_RDWR, S_IRWXU);
    ftruncate(shared_memory, length * sizeof(int));

    // if (errno != EEXIST) { // nur machen wenn noch nicht existent
    //     ftruncate(shared_memory, length * sizeof(int));
    // }

    int * const shared_mem_pointer = mmap(NULL, length * sizeof(int), PROT_WRITE, MAP_SHARED, shared_memory, 0);
    if (shared_mem_pointer == (int * const) MAP_FAILED) {
        fprintf(stderr, "mmap failed in sender\n");
        // error
    }
    
    if (close(shared_memory) == -1) {
        // error while closing
    }

    int c = '\0'; // char für zeichenweises lesen/schreiben
    int i = 0;

    while (TRUE) {
        (void) sem_wait(sem_empty); // return value -1 and errno == EINTR prüfen

        c = getchar(); 

        // critical section
        (void) sem_wait(sem_mutex); // same
        *(shared_mem_pointer + i) = c;
        (void) sem_post(sem_mutex); // error-checking noch einbauen
        // end critical section

        i++;
        i %= length;

        (void) sem_post(sem_full); // same

        if (c == EOF) {
            break;
        }
    }

    if (sem_close(sem_mutex) == -1) {
        // error
    }

    if (sem_close(sem_full) == -1) {
        // error
    }

    if (sem_close(sem_empty) == -1) {
        // error
    }

    if (munmap(shared_mem_pointer, length * sizeof(int)) == -1) {
        // error
    }

    // kein sem_unlink(), da das nur ein Prozess machen sollte
    // kein shm_unlink(), da das nur ein Prozess machen sollte

    return EXIT_SUCCESS;
} // end main

