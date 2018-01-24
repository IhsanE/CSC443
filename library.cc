#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void random_array(char *array, long bytes) {
    srand(time(NULL));
    for (int i = 0; i < bytes; ++i) {
        char r_char = 'A' + (rand() % 26);
        array[i] = r_char;
    }
}
