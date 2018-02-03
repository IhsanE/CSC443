#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sstream>
#include <istream>
#include <sys/timeb.h>
#include <cstring>
#include "library.h"
#include <typeinfo>

using namespace std;


int main(int argc, const char * argv[]) {
    if (argc < 3) {
        cout << "Usage: write_fixed_len_pages <page_file> <page_size>" << endl;
        return 1;
    }
    string page_file_name(argv[1]);
    int page_size = stoi(argv[2]);

    // Open the page file for reading
    ifstream page_file;
    page_file.open(page_file_name, ios::in | ios::binary);

    Page *page;

    while (!page_file.eof()) {
        cout << "HI" << endl;
        page = (Page *)malloc(sizeof(Page));

        init_fixed_len_page(page, page_size, NUM_ATTR*ATTR_SIZE);

        page_file.read((char *) page->data, page_size);

        for (int slot = 0; slot < fixed_len_page_capacity(page); slot++) {
            Record r;
            read_fixed_len_page(page, slot, &r);
            for (Record::iterator iter = (r).begin(); iter != (r).end(); iter++) {
                if (r.end() - iter == 1) {
                    cout << *iter << endl;
                } else {
                    cout << *iter << ",";
                }
            }
        }
        free(page);
    }

    page_file.close();

    /* cout << "TIME: " << stop_ms - start_ms << " milliseconds" << endl; */

    return 0;
}
