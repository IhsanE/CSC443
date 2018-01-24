#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>

void random_array(char *array, long bytes) {
    srand(time(NULL));
    for (int i = 0; i < bytes; ++i) {
        char r_char = 'A' + (rand() % 26);
        array[i] = r_char;
    }
}

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
