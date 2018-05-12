#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "library.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "Error, usage must be:" << endl;
        cout << "./delete <heapfile> <record_id> <page_size>" << endl;
        cout << "<record_id> format: <page_id>-<slot_index>";
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
    // don't include hyphen
    rid->slot = atoi(record_id.substr(split_position + 1).c_str());

    int page_size = atoi(argv[3]);

    // Prep heapfile
    Heapfile *heap = new Heapfile();
    heap->page_size = page_size;
    heap->file_ptr = heapfile;

    // Prep page
    Page *page = (Page *)malloc(sizeof(Page));
    init_fixed_len_page(page, heap->page_size, NUM_ATTR*ATTR_SIZE);

    // Create null record to mimic deleting
    Record null_record = Record();
    void *null_buff = malloc(NUM_ATTR*ATTR_SIZE);
    memset(null_buff, 0, NUM_ATTR*ATTR_SIZE);
    fixed_len_read(null_buff, (NUM_ATTR*ATTR_SIZE), &null_record);

    // Load page data
    read_page(heap, rid->page_id, page);
    // Write null record to page
    write_fixed_len_page(page, rid->slot, &null_record);
    // Mark bit as free in slotted page directory
    mark_slot_clean(page, rid->slot);

    write_page(page, heap, rid->page_id);
    fclose(heap->file_ptr);

    return 0;
}
