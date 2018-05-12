#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include "library.h"

using namespace std;
const int SUBSTRING_LENGTH = 5;

int main(int argc, char** argv) {
    if (argc < 7) {
        cout << "Error, usage must be:" << endl;
        cout << "./select3 <colstore_name> <attribute_id> <return_attribute_id> <start> <end> <page_size>" << endl;
        return 1;
    }

    char *colstore_directory = argv[1];
    int attribute_id = atoi(argv[2]);
    int return_attribute_id = atoi(argv[3]);
    const char *start = argv[4];
    const char *end = argv[5];
    int page_size = atoi(argv[6]);

    // Get the heapfile containig all attributes of attribute_id
    Heapfile *heap = new Heapfile();
    ostringstream heap_file_name;
    heap_file_name << colstore_directory << "/" << attribute_id;
    FILE* heap_file_ptr = fopen(heap_file_name.str().c_str(), "r");
    heap->page_size = page_size;
    heap->file_ptr = heap_file_ptr;

    // Get the heapfile containing all attributes of return_attribute_id
    Heapfile *return_heap = new Heapfile();
    ostringstream return_heap_file_name;
    return_heap_file_name << colstore_directory << "/" << return_attribute_id;
    FILE* return_heap_file_ptr = fopen(return_heap_file_name.str().c_str(), "r");
    return_heap->page_size = page_size;
    return_heap->file_ptr = return_heap_file_ptr;


    RecordIterator record_iterator = RecordIterator(heap);
    Record r = Record();
    BoostedRecord boosted_record = BoostedRecord();

    RecordIterator record_return_iterator = RecordIterator(return_heap);
    Record r_return = Record();
    BoostedRecord boosted_record_return = BoostedRecord();


    set<int> matches_idx;
    int match_count = 0;

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
                matches_idx.insert(match_count);
            }
            match_count++;
        }
    }

    match_count = 0;
    while (record_return_iterator.hasNext()) {
        boosted_record_return = record_return_iterator.next();
        r_return = *(boosted_record_return.record);

        for (Record::iterator iter = (r_return).begin(); iter != (r_return).end(); iter++) {
            if (matches_idx.find(match_count) != matches_idx.end()) {
                char *substring = new char[SUBSTRING_LENGTH + 1];
                substring[SUBSTRING_LENGTH] = '\0';
                strncpy(substring, *iter, SUBSTRING_LENGTH);
                cout << substring << endl;
                matches_idx.insert(match_count);
            }
            match_count++;
        }
    }

    ftime(&t);
    long total_run_time = ((t.time * 1000) + t.millitm) - start_time_in_ms;
    cout << "RUN TIME: " << total_run_time << " ms" << endl;

    fclose(heap->file_ptr);
    fclose(return_heap->file_ptr);

    return 0;
}
