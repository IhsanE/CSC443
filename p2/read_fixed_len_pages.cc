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
        cout << "Usage: write_fixed_len_pages <page_file> <page_size>" << endl;
        return 1;
    }
    std::string page_file_name(argv[1]);
    int page_size = std::stoi(argv[3]);

    // Open the page file for reading
    std::ofstream page_file;
    page_file.open(page_file_name);

    // Read the CSV file line-by-line:
    std::string record;
    std::string record_copy;

    int num_pages = 0;
    int num_records = 0;
    Page *page = (Page *)malloc(sizeof(Page));


    while (!page_file.eof()) {
        init_fixed_len_page(page, page_size, NUM_ATTR*ATTR_SIZE);

        page_file.read((char *) page->data, page_size);
    }

    if (fixed_len_page_freeslots(page) > 0) {
        page_file.write((const char *) page->data, page->page_size);
    }

    page_file.close();

    cout << "NUMBER OF RECORDS: " << num_records << endl;
    cout << "NUMBER OF PAGES: " << num_pages << endl;
    cout << "TIME: " << stop_ms - start_ms << " milliseconds" << endl;

    return 0;
}
