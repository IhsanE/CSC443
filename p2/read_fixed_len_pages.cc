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

    struct timeb t;
    ftime(&t);
    unsigned long start_ms = t.time * 1000 + t.millitm;

    while (!page_file.eof()) {
        page = (Page *)malloc(sizeof(Page));
        init_fixed_len_page(page, page_size, NUM_ATTR*ATTR_SIZE);

        char *buff = (char *) malloc((page->num_records * NUM_ATTR*ATTR_SIZE) + 1);
        memset(buff, 0, (page->num_records * NUM_ATTR*ATTR_SIZE) + 1);
        page_file.read(buff, (page->num_records * NUM_ATTR*ATTR_SIZE));

        for (int i = 0; i < fixed_len_page_capacity(page); i++) {
            Record temp_r = Record();
            fixed_len_read(buff + NUM_ATTR*ATTR_SIZE*i, NUM_ATTR*ATTR_SIZE, &temp_r);
            add_fixed_len_page(page, &temp_r);
            for (Record::iterator iter = (temp_r).begin(); iter != (temp_r).end(); iter++) {
                cout << *iter;
            }
            cout << endl;
        }
        free(buff);
        free(page->data);
        free(page);
    }

    page_file.close();

	ftime(&t);
    unsigned long stop_ms = t.time * 1000 + t.millitm;

    cout << "TIME: " << stop_ms - start_ms << " milliseconds" << endl;

    return 0;
}
