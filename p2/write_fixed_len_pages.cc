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
    page_file.open(page_file_name, std::ios::out | std::ios::binary);

    // Read the CSV file line-by-line:
    std::ifstream csv_file(csv_file_name);
    std::string record;
    std::string record_copy;

    while (std::getline(csv_file, record)) {
        record_copy = record;
        Record r = Record();
        std::stringstream ss(record_copy);
        std::string record_attr;
        while (std::getline(ss, record_attr, ',')) {
            r.push_back(const_cast<char*>(record_attr.c_str()));
            cout << const_cast<char*>(record_attr.c_str()) << endl;
        }

    }

    return 0;
}
