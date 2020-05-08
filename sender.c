#include "sender_empfaenger.h"


int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage:\n");
        return EXIT_FAILURE;
    }

    int opt;
    int length;

    while ((opt = getopt(argc, argv, "m:")) != -1) {
        switch (opt) {
            case 'm':
                length = strtol(optarg, 0, 10);
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

    sem_t * const sem_mutex = sem_open(sem_name_1, O_CREAT | O_EXCL, S_IRWXU, 1);
    if (sem_mutex == SEM_FAILED) {
        // error in sem_open()
    }
    sem_t * const sem_full  = sem_open(sem_name_2, O_CREAT | O_EXCL, S_IRWXU, 0);
    if (sem_full == SEM_FAILED) {
        // error in sem_open()
    }
    sem_t * const sem_empty = sem_open(sem_name_3, O_CREAT | O_EXCL, S_IRWXU, length); // vl length - 1
    if (sem_empty == SEM_FAILED) {
        // error in sem_open()
    }

    errno = 0;
    const int shared_memory = shm_open(shm_name_0, O_CREAT | O_EXCL | O_RDWR, S_IRWXU);
    if (errno != EEXIST) { // nur machen wenn noch nicht existent
        ftruncate(shared_memory, length * sizeof(int));
    }

    int * const shared_mem_pointer = mmap(NULL, length * sizeof(int), PROT_WRITE, MAP_SHARED, shared_memory, 0);
    if (close(shared_memory) == -1) {
        // error while closing
    }

    int c = '\0';
    int i = 0;

    while (TRUE) {
        (void) sem_wait(sem_empty); // return value -1 and errno == EINTR prüfen
        (void) sem_wait(sem_mutex); // same
        // critical section
        c = getchar(); 
        *(shared_mem_pointer + i) = c;

        i++;
        i %= length;

        if (c == EOF) {
            break;
        }
        // end critical section
        (void) sem_post(sem_mutex);
        (void) sem_post(sem_full);
    }

    return EXIT_SUCCESS;
} // end main

