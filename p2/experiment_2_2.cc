#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "library.h"
using namespace std;


int main () {
    Page *page = (Page *)malloc(sizeof(Page));
    init_fixed_len_page(page, 1000, 100);
    void *buf = malloc(40);
    memset(buf, 0, 40);
    *((int*)buf) = 3;
    cout << *((int *)page->directory) << endl;
    cout << page << endl;
    cout << fixed_len_page_freeslots(page) << endl;
    cout << find_first_free_slot(page) << endl;
}
