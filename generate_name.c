#include "generate_name.h"


char *generate_name(char *prefix, uid_t id, int offset) {
    id *= 1000;
    id += offset;

    int digits = (int) log10((double) id) + 1;
    char *id_string = calloc(digits + 1, sizeof(char));

    int i = 0;
    while (id) {
        id_string[i] = id % 10 + 48;
        i++;
        id /= 10;
    }
    id_string[digits] = '\0';

    id_string = reverse_string(id_string);

    strcat(prefix, id_string);
    free(id_string);

    return prefix;
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

