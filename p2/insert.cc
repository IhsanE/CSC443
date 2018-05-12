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
        cout << "./insert <heapfile> <csv_file> <page_size>\n";
        return 1;
    }

    FILE *heapfile = fopen(argv[1], "r+");
    if (!heapfile) {
        cout << "Error, could not find file " << argv[1] << "\n";
        return 1;
    }

    string csv_file_name(argv[2]);
    string record;
    std::ifstream csv_file(csv_file_name);
    if (!csv_file) {
        cout << "Error, could not find file " << argv[2] << "\n";
        return 1;
    }

    int page_size = atoi(argv[3]);

    Heapfile *heap = new Heapfile();
    heap->page_size = page_size;
    heap->file_ptr = heapfile;

    fseek(heapfile, 0, SEEK_END);
    unsigned long len = (unsigned long) ftell(heapfile);

    int num_dirs;
    fseek(heap->file_ptr, 0, SEEK_SET);
    fread(&num_dirs, sizeof(uint8_t), 1, heap->file_ptr);
    fseek(heap->file_ptr, 0, SEEK_END);

    if (len == 0) {
        cout << "Error, the heapfile is empty" << endl;
        return 1;
    }

    Page *page = (Page *)malloc(sizeof(Page));
    init_fixed_len_page(page, heap->page_size, NUM_ATTR*ATTR_SIZE);
    PageID pid = alloc_page(heap);

    while (getline(csv_file, record)) {
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
            // Write the full page
            write_page(page, heap, pid);

            // Init a new page
            free(page->data);
            pid++;
            init_fixed_len_page(page, heap->page_size, NUM_ATTR*ATTR_SIZE);
            // Write record to page
            add_fixed_len_page(page, &r);
        }
    }
    if (fixed_len_page_freeslots(page) != 0) {write_page(page, heap, pid);}
    csv_file.close();
    fclose(heapfile);
    return 0;
}
