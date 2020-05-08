#include "generate_name.h"


char *generate_name(char *prefix, uid_t id, int offset) {
    id *= 1000;
    id += offset;

    int digits = (int) log10((double) id) + 1;
    char *id_name = calloc(digits + 1, sizeof(char));
    
    int i = 0;
    while (id) {
        id_name[i] = id % 10 + 48;
        i++;
        id /= 10;
    }
    id_name[digits] = '\0';

    id_name = reverse_string(id_name);

    strcat(prefix, id_name);
    free(id_name);

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

