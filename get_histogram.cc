#include <stdio.h>
#include <stdlib.h>

#include "library.h"


int main (int argc, char *argv[]){
    long block_size = atoi(argv[2]);
    FILE *fp = fopen(argv[1], "r");
    long hist[26] = {0};
    long milliseconds = 0;
    long total_bytes_read;

    fseek(fp, 0L, SEEK_END);
    total_bytes_read = ftell(fp);
    rewind(fp);
    get_histogram(fp, hist, block_size, &milliseconds, &total_bytes_read);
    fclose(fp);
    for (int i = 0; i < 26; i++) {
        printf("%c: %ld\n", 'A' + i, hist[i]);
    }
    printf("BLOCK SIZE %ld bytes\n", block_size);
    printf("TOTAL BYTES %ld bytes\n", total_bytes_read);
    printf("TIME %ld ms.\n", milliseconds);
    printf("DATA RATE: %f Bps\n", (double)total_bytes_read/milliseconds * 1000);
}
