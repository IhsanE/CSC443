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
    if (argc < 6) {
        cout << "Error, usage must be:\n";
        cout << "./update <heapfile> <record_id> <attribute_id> <new_value> <page_size>\n";
        return 1;
    }

    FILE *heapfile = fopen(argv[1], "r+");
    if (!heapfile) {
        cout << "Error, could not find file " << argv[1] << "\n";
        return 1;
    }

    string record_id = argv[2];
    size_t split_position = record_id.find('-');

    RecordID *rid = new RecordID();
    rid->page_id = atoi(record_id.substr(0, split_position).c_str());
    rid->slot = atoi(record_id.substr(split_position + 1).c_str());

    int attribute_id = atoi(argv[3]);

    char *new_value = argv[4];
    if (strlen(new_value) != ATTR_SIZE) {
        cout << "Attributes are: " << ATTR_SIZE << " characters" << endl;
        return 1;
    }

    int page_size = atoi(argv[5]);

    // Prep heapfile
    Heapfile *heap = new Heapfile();
    heap->page_size = page_size;
    heap->file_ptr = heapfile;

    // Prep page
    Page *page = (Page *)malloc(sizeof(Page));
    init_fixed_len_page(page, heap->page_size, NUM_ATTR*ATTR_SIZE);

    // Create record to change
    Record record_to_change = Record();

    // Load page data
    read_page(heap, rid->page_id, page);
    // Load record from page
    read_fixed_len_page(page, rid->slot, &record_to_change);
    // Modify record value
    if (attribute_id < (int) record_to_change.size()) {
        record_to_change.at(attribute_id) = new_value;
    } else {
        cout << "Attribute id not found" << endl;
        return -1;
    }
    // Write changed record to page
    write_fixed_len_page(page, rid->slot, &record_to_change);

    write_page(page, heap, rid->page_id);
    fclose(heap->file_ptr);

    return 0;
}
