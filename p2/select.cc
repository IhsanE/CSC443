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
const int SUBSTRING_LENGTH = 5;


int main(int argc, const char * argv[]) {
    if (argc < 6) {
        cout << "Error, usage must be:\n";
        cout << "./select <heapfile> <attribute_id> <start> <end> <page_size>\n";
        return 1;
    }

    FILE* heapfile = fopen(argv[1], "rb");
    if (!heapfile) {
        cout << "Error, could not find file " << argv[1] << "\n";
        return 1;
    }

    int attribute_id = atoi(argv[2]);
    const char *start = argv[3];
    const char *end = argv[4];
    int page_size = atoi(argv[5]);

    Heapfile *heap = new Heapfile();
    heap->page_size = page_size;
    heap->file_ptr = heapfile;

    RecordIterator record_iterator = RecordIterator(heap);
    Record r = Record();
    BoostedRecord boosted_record = BoostedRecord();


    struct timeb t;
    ftime(&t);
    long start_time_in_ms = (t.time * 1000) + t.millitm;

    while (record_iterator.hasNext()) {
        boosted_record = record_iterator.next();
        r = *(boosted_record.record);
        if (
            attribute_id < (int) r.size() &&
            strcmp(r.at(attribute_id), start) >= 0 &&
            strcmp(r.at(attribute_id), end) <= 0
        ) {
            char *substring = new char[SUBSTRING_LENGTH + 1];
            substring[SUBSTRING_LENGTH] = '\0';
            strncpy(substring, r.at(attribute_id), SUBSTRING_LENGTH);
            cout << substring << endl;
        }
        cout << endl;
    }

    ftime(&t);
    long total_run_time = ((t.time * 1000) + t.millitm) - start_time_in_ms;
    cout << "RUN TIME: " << total_run_time << " ms" << endl;
}
