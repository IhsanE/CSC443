#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include <cstring>
#include "library.h"
#include <typeinfo>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "Error, usage must be:\n";
        cout << "./insert <csv_file> <heapfile> <page_size>\n";
        return 1;
    }

    FILE *heapfile = fopen(argv[2], "w+");
    if (!heapfile) {
        cout << "Error, could not find file " << argv[2] << "\n";
        return 1;
    }

    string csv_file_name(argv[1]);
    string record;
    std::ifstream csv_file(csv_file_name);
    if (!csv_file) {
        cout << "Error, could not find file " << argv[1] << "\n";
        return 1;
    }

    int page_size = atoi(argv[3]);

    Heapfile *heap = new Heapfile();
    heap->page_size = page_size;
    heap->file_ptr = heapfile;
    init_heapfile(heap, heap->page_size, heap->file_ptr);


    Page *page = (Page *)malloc(sizeof(Page));
    init_fixed_len_page(page, heap->page_size, NUM_ATTR*ATTR_SIZE);
    PageID pid = 0;

    struct timeb t;
    ftime(&t);
    long start_time_in_ms = (t.time * 1000) + t.millitm;
    long total_run_time = 0;

    int num_records = 0;

    int num_writes = 0;

    while (getline(csv_file, record)) {
        num_records++;
        Record r = Record();
        stringstream ss(record);
        string record_attr;
        while (std::getline(ss, record_attr, ',')) {
            char *record_copy = (char *) malloc(ATTR_SIZE + 1);
            std::strncpy(record_copy, record_attr.c_str(), ATTR_SIZE + 1);
            r.push_back(record_copy);
        }

        int page_full = add_fixed_len_page(page, &r);
        if (page_full == -1) {
            ftime(&t);
            start_time_in_ms = (t.time * 1000) + t.millitm;

            // Write the full page
            num_writes++;
            write_page(page, heap, pid);
            ftime(&t);
            long write_run_time = ((t.time * 1000) + t.millitm) - start_time_in_ms;
            total_run_time += write_run_time;

            // Init a new page
            free(page->data);
            pid++;
            init_fixed_len_page(page, heap->page_size, NUM_ATTR*ATTR_SIZE);
            // Write record to page
            add_fixed_len_page(page, &r);
        }
    }
    if (fixed_len_page_freeslots(page) != 0) {
        ftime(&t);
        start_time_in_ms = (t.time * 1000) + t.millitm;

        // Write the full page
        write_page(page, heap, pid);
        ftime(&t);
        long write_run_time = ((t.time * 1000) + t.millitm) - start_time_in_ms;
        total_run_time += write_run_time;
    }

    cout << "num writes: " << num_writes << endl;

    cout << "RUN TIME: " << total_run_time << " ms" << endl;
    cout << "TOTAL RECORDS: " << num_records << endl;
    cout << "RECORD/ms: " << (float)num_records/(float)total_run_time << endl;

    csv_file.close();
    fclose(heapfile);
    return 0;
}
