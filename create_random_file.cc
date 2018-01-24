#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "library.h"


int main (int argc, char *argv[]) {
    long block_size = atoi(argv[3]);
    long size = atoi(argv[2]);
    FILE *fp = fopen(argv[1], "w");
    long milliseconds = 0;
    long t_before_ms = 0;
    long t_after_ms = 0;

    char buf[block_size];

    int unwritten_chars = size;
    struct timeb t;
    while (unwritten_chars > 0) {
        random_array(buf, block_size);
        t_before_ms = 0;
        t_after_ms = 0;
        if (unwritten_chars < block_size) {
            ftime(&t);
            t_before_ms = t.time * 1000 + t.millitm;
            fwrite(buf, 1, unwritten_chars, fp);
            fflush(fp);
            ftime(&t);
            t_after_ms = t.time * 1000 + t.millitm;
        } else {
            ftime(&t);
            t_before_ms = t.time * 1000 + t.millitm;
            fwrite(buf, 1, block_size, fp);
            fflush(fp);
            ftime(&t);
            t_after_ms = t.time * 1000 + t.millitm;
        }
        milliseconds += t_after_ms - t_before_ms;
        unwritten_chars -= block_size;
    }

    printf("TIME %ld ms.\n", milliseconds);
    printf("DATA RATE: %f Bps\n", (double)size/milliseconds * 1000);
    fclose(fp);
}
