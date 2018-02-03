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
    if (argc < 4) {
        cout << "Usage: write_fixed_len_pages <csv_file> <page_file> <page_size>" << endl;
        return 1;
    }
    std::string csv_file_name(argv[1]);
    std::string page_file_name(argv[2]);
    int page_size = std::stoi(argv[3]);

    // Open the page file for writing
    std::ofstream page_file;
    page_file.open(page_file_name);

    // Read the CSV file line-by-line:
    std::ifstream csv_file(csv_file_name);
    std::string record;
    std::string record_copy;

    int num_pages = 0;
    int num_records = 0;
    Page *page = (Page *)malloc(sizeof(Page));

    struct timeb t;
    ftime(&t);
    unsigned long start_ms = t.time * 1000 + t.millitm;

    while (std::getline(csv_file, record)) {
        Record r = Record();
        std::stringstream ss(record);
        std::string record_attr;
        while (std::getline(ss, record_attr, ',')) {
            char *record_copy = (char *) malloc(ATTR_SIZE + 1);
            std::strncpy(record_copy, record_attr.c_str(), ATTR_SIZE + 1);
            r.push_back(record_copy);
        }
        if (num_pages == 0) {
            init_fixed_len_page(page,  page_size, fixed_len_sizeof(&r));
            num_pages++;
        }
        int page_full = add_fixed_len_page(page, &r);
        num_records++;
        if (page_full == -1) {
            page_file.write((const char *) page->data, page->page_size - fixed_len_sizeof(&r));
            init_fixed_len_page(page,  page_size, fixed_len_sizeof(&r));
            num_pages++;
            add_fixed_len_page(page, &r);
        }
    }

    if (fixed_len_page_freeslots(page) > 0) {
        page_file.write((const char *) page->data, page->page_size);
    }

    page_file.close();

	ftime(&t);
    unsigned long stop_ms = t.time * 1000 + t.millitm;

    cout << "NUMBER OF RECORDS: " << num_records << endl;
    cout << "NUMBER OF PAGES: " << num_pages << endl;
    cout << "TIME: " << stop_ms - start_ms << " milliseconds" << endl;

    return 0;
}
