#include "generate_name.h"
#include "sender_empfaenger.h"
#include <math.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


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

    printf("%s\n", shm_name_0);
    printf("%s\n", sem_name_1);
    printf("%s\n", sem_name_2);
    printf("%s\n", sem_name_3);

    return 0;
} // end main

