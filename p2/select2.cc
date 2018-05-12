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
const int SUBSTRING_LENGTH = 5;

int main(int argc, char** argv) {
    if (argc < 6) {
        cout << "Error, usage must be:" << endl;
        cout << "./select2 <colstore_name> <attribute_id> <start> <end> <page_size>" << endl;
        return 1;
    }

    char *colstore_directory = argv[1];

    int attribute_id = atoi(argv[2]);
    const char *start = argv[3];
    const char *end = argv[4];
    int page_size = atoi(argv[5]);

    // Get the heapfile containig all attributes of attribute_id
    Heapfile *heap = new Heapfile();
    ostringstream heap_file_name;
    heap_file_name << colstore_directory << "/" << attribute_id;
    FILE* heap_file_ptr = fopen(heap_file_name.str().c_str(), "r");
    heap->page_size = page_size;
    heap->file_ptr = heap_file_ptr;


    RecordIterator record_iterator = RecordIterator(heap);
    Record r = Record();
    BoostedRecord boosted_record = BoostedRecord();

    struct timeb t;
    ftime(&t);
    long start_time_in_ms = (t.time * 1000) + t.millitm;

    while (record_iterator.hasNext()) {
        boosted_record = record_iterator.next();
        r = *(boosted_record.record);

        for (Record::iterator iter = (r).begin(); iter != (r).end(); iter++) {
            if (
                strcmp(*iter, start) >= 0 &&
                strcmp(*iter, end) <= 0
            ) {
                char *substring = new char[SUBSTRING_LENGTH + 1];
                substring[SUBSTRING_LENGTH] = '\0';
                strncpy(substring, *iter, SUBSTRING_LENGTH);
                cout << substring << endl;
            }
        }
    }

    ftime(&t);
    long total_run_time = ((t.time * 1000) + t.millitm) - start_time_in_ms;
    cout << "RUN TIME: " << total_run_time << " ms" << endl;

    fclose(heap->file_ptr);

    return 0;
}
