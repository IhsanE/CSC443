#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include "library.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "Error, usage must be:" << endl;
        cout << "./csv2colstore <csv_file> <colstore_name> <page_size>" << endl;
        return 1;
    }

    ifstream csv_file;
    csv_file.open(argv[1]);
    if (!csv_file) {
        cout << "Error, could not find file " << argv[1] << "\n";
        return 1;
    }

    char *colstore_directory = argv[2];
    if (mkdir(colstore_directory, 0700) != 0) {
        cout << "Error, could not create directory " << colstore_directory << "\n";
        return 1;
    }

    int page_size = atoi(argv[3]);

    struct timeb t;
    ftime(&t);
    long start_time_in_ms = (t.time * 1000) + t.millitm;
    long total_run_time = 0;
    int num_records = 0;

    string record;
    vector<Record> col_records = vector<Record>();
    vector<Heapfile *> heaps = vector<Heapfile *>();
    vector<Page *> pages = vector<Page *>();

    // Initialize vectors to store records, heaps, and pages
    for (int i = 0; i < NUM_ATTR; i++) {
        Record r = Record();
        Heapfile *heap = new Heapfile();
        Page *page = (Page *)malloc(sizeof(Page));
        ostringstream heap_file_name;
        heap_file_name << colstore_directory << "/" << i;
        FILE* heap_file_ptr = fopen(heap_file_name.str().c_str(), "w+");
        heap->page_size = page_size;
        heap->file_ptr = heap_file_ptr;
        init_heapfile(heap, heap->page_size, heap->file_ptr);
        init_fixed_len_page(page, heap->page_size, NUM_ATTR*ATTR_SIZE);
        pages.push_back(page);
        heaps.push_back(heap);
        col_records.push_back(r);
    }


    while (getline(csv_file, record)) {
        num_records++;
        stringstream ss(record);
        string record_attr;

        vector<Record>::iterator col_iter = col_records.begin();
        // Fill up our vector of vectors of records with each attribute
        // Each element of col_records is a record pertaining to ONE attribute
        while (getline(ss, record_attr, ',')) {
            char *record_copy = (char *) malloc(ATTR_SIZE + 1);
            strncpy(record_copy, record_attr.c_str(), ATTR_SIZE + 1);
            (*col_iter).push_back(record_copy);
            col_iter++;
        }

        // After we fill a "record" for each attribute, we write to page
        if (col_records.at(0).size() == 100) {
            // Write each attribute record to corresponding attribute page
            for (vector<Record>::iterator iter = (col_records).begin(); iter != (col_records).end(); iter++) {
                int cur_index = iter - (col_records).begin();
                int page_full = add_fixed_len_page(pages.at(cur_index), &(*iter));

                if (page_full == -1) {
                    // Write attribute page to attribute heap
                    PageID pid = alloc_page(heaps.at(cur_index));
                    // Write the full page
                    ftime(&t);
                    start_time_in_ms = (t.time * 1000) + t.millitm;
                    write_page(pages.at(cur_index), heaps.at(cur_index), pid);
                    ftime(&t);
                    long write_run_time = ((t.time * 1000) + t.millitm) - start_time_in_ms;
                    total_run_time += write_run_time;

                    // Init a new page
                    free(pages.at(cur_index)->data);
                    init_fixed_len_page(pages.at(cur_index), heaps.at(cur_index)->page_size, NUM_ATTR*ATTR_SIZE);
                    // Write record to page
                    add_fixed_len_page(pages.at(cur_index), &(*iter));
                }
            }

            // Clear all record vectors
            for (vector<Record>::iterator iter = (col_records).begin(); iter != (col_records).end(); iter++) {
                (*iter).clear();
            }
        }
    }
    csv_file.close();

    // If our last page has not been written, then write it!
    if (fixed_len_page_freeslots(pages.at(0)) != 0) {
        for (vector<Page *>::iterator iter = (pages).begin(); iter != (pages).end(); iter++) {
            int cur_index = iter - (pages).begin();
            PageID pid = alloc_page(heaps.at(cur_index));
            ftime(&t);
            start_time_in_ms = (t.time * 1000) + t.millitm;
            write_page((*iter), heaps.at(cur_index), pid);
            ftime(&t);
            long write_run_time = ((t.time * 1000) + t.millitm) - start_time_in_ms;
            total_run_time += write_run_time;
        }
    }

    // Close all heap file pointers
    for (vector<Heapfile *>::iterator iter = (heaps).begin(); iter != (heaps).end(); iter++) {
        fclose((*iter)->file_ptr);
    }

    cout << "RUN TIME: " << total_run_time << " ms" << endl;
    cout << "TOTAL RECORDS: " << num_records << endl;
    cout << "RECORD/ms: " << (float)num_records/(float)total_run_time << endl;

    return 0;
}
