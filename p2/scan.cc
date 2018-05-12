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

int main(int argc, const char * argv[]) {
	if (argc < 3) {
        cout << "Error, usage must be:\n";
        cout << "./scan <heapfile> <page_size>\n";
        return 1;
    }

    FILE* heapfile = fopen(argv[1], "rb");
    if (!heapfile) {
        cout << "Error, could not find file " << argv[1] << "\n";
        return 1;
    }

    int page_size = atoi(argv[2]);

    Heapfile *heap = new Heapfile();
    heap->page_size = page_size;
    heap->file_ptr = heapfile;
    init_heapfile(heap, heap->page_size, heap->file_ptr);

    Page *page = (Page *)malloc(sizeof(Page));
    init_fixed_len_page(page, heap->page_size, NUM_ATTR*ATTR_SIZE);

    RecordIterator record_iterator = RecordIterator(heap);
    Record r = Record();
    BoostedRecord boosted_record = BoostedRecord();
    while (record_iterator.hasNext()) {
        boosted_record = record_iterator.next();
        r = *(boosted_record.record);
        cout << "RecordID: " << boosted_record.id->page_id << "-" << boosted_record.id->slot << endl;
        for (Record::iterator iter = (r).begin(); iter != (r).end(); iter++) {
            cout << *iter << " ";
        }
        cout << endl;
    }
}
