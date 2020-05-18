#include "sender_empfaenger.h"


unsigned int arguments(int argc, char **argv) {
    unsigned int length;
    int opt;
	
	if(argc != 3) { // check Eingabe
        fprintf(stderr, "Usage: %s [-m] size\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, ":m:")) != -1) {
        switch (opt) {
            case 'm':
                length = (unsigned int) strtol(optarg, 0, 10); // error-checking einbauen
                // printf("%d\n", length);
                break;
            default:
				fprintf(stderr, "Usage: %s [-m] size\n", argv[0]);
				exit(EXIT_FAILURE);
        }
    }
	
	
    if (optind != argc) { // check Eingabe
        fprintf(stderr, "Usage: %s [-m] size\n", argv[0]);
		exit(EXIT_FAILURE);
    }

    return length;
} // end arguments


void make_names(void)  {
    const uid_t id = getuid();
    strcat(shm_name_0, "/shm_");
    strcat(sem_name_1, "/sem_");
    strcat(sem_name_2, "/sem_");
    strcat(sem_name_3, "/sem_");
    (void) generate_name(shm_name_0, id, 0);
    (void) generate_name(sem_name_1, id, 1);
    (void) generate_name(sem_name_2, id, 2);
    (void) generate_name(sem_name_3, id, 3);
} // end make_name
    

char *generate_name(char *name, uid_t id, const int offset) {
    id *= 1000;
    id += offset;

    int num_digits = (int) log10((double) id) + 1;
    char *id_string = calloc(num_digits + 1, sizeof(char));

    int i = 0;
    while (id) {
        id_string[i] = id % 10 + 48;
        i++;
        id /= 10;
    }
    id_string[num_digits] = '\0';

    id_string = reverse_string(id_string);

    strcat(name, id_string);
    free(id_string);

    return name;
} // end generate_name


char *reverse_string(char *string) {
    int length = strlen(string);
    char temp;

    for (int i = 0; i < length / 2; i++) {
        temp = string[i];
        string[i] = string[length - 1 - i];
        string[length - 1 - i] = temp;
    }

    return string;
} // end reverse string


sem_t *sem_open_error_checked(const char *name, int oflag, mode_t mode, unsigned int value) {
    sem_t *sem_pointer = sem_open(name, oflag, mode, value);

    if (sem_pointer == SEM_FAILED) {
        fprintf(stderr, "sem_open failed\n");
        // error in sem_open() und exit
    }

    return sem_pointer;
} // end sem_open_error_checked


void close_all(int shared_memory, sem_t *sem_mutex, sem_t *sem_full, sem_t *sem_empty) {
    if (close(shared_memory) == -1) {
        // error while closing fd
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
} // end close_all


void unlink_sem(char *sem_name_1, char *sem_name_2, char *sem_name_3) {
    if (sem_unlink(sem_name_1) == -1) {
        // error
    }

    if (sem_unlink(sem_name_2) == -1) {
        // error
    }

    if (sem_unlink(sem_name_3) == -1) {
        // error
    }
} // end unlink_sem



