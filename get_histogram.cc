#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "library.h"



void compute_histogram(long *p_hist, char *p_buf,int size) {
    for (long i = 0; i < size; i++) {
        long i_letter = p_buf[i] - 'A';
        p_hist[i_letter] += 1;
    }
}

int get_histogram(
    FILE *fp,
    long hist[],
    int block_size,
    long *milliseconds,
    long *total_bytes_read
) {
    int unread_chars = *total_bytes_read;
    char buf[block_size];
    char *p_buf = buf;
    long *p_hist = hist;
    struct timeb t;
    while (unread_chars > 0) {
        bzero(buf, block_size);
        ftime(&t);
        long t_before_ms = t.time * 1000 + t.millitm;
        fread(buf, 1, block_size, fp);
        ftime(&t);
        long t_after_ms = t.time * 1000 + t.millitm;
        *milliseconds += t_after_ms - t_before_ms;
        compute_histogram(p_hist, p_buf, block_size);
        unread_chars -= block_size;
    }
    fclose(fp);
}

int main (int argc, char *argv[]){
    long block_size = atoi(argv[2]);
    FILE *fp = fopen(argv[1], "r");
    long hist[26] = {0};
    long milliseconds;
    long total_bytes_read;

    fseek(fp, 0L, SEEK_END);
    total_bytes_read = ftell(fp);
    rewind(fp);
    get_histogram(fp, hist, block_size, &milliseconds, &total_bytes_read);
    for (int i = 0; i < 26; i++) {
        printf("%c: %ld\n", 'A' + i, hist[i]);
    }
    printf("BLOCK SIZE %ld bytes\n", block_size);
    printf("TOTAL BYTES %ld bytes\n", total_bytes_read);
    printf("TIME %ld ms.\n", milliseconds);
    printf("DATA RATE: %f Bps\n", (double)total_bytes_read/milliseconds * 1000);
}
