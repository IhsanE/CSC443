#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "library.h"


int main (int argc, char *argv[]) {
    long block_size = atoi(argv[3]);
    long size = atoi(argv[2]);
    FILE *fp = fopen(argv[1], "w");

    char buf[block_size];

    int unwritten_chars = size;
    while (unwritten_chars > 0) {
        random_array(buf, block_size);
        if (unwritten_chars < block_size) {
            fwrite(buf, 1, unwritten_chars, fp);
            fflush(fp);
        } else {
            fwrite(buf, 1, block_size, fp);
            fflush(fp);
        }
        unwritten_chars -= block_size;
    }

    fclose(fp);
}
