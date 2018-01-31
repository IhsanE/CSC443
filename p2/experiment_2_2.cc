#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "library.h"
using namespace std;


int main () {
    Page *page = (Page *)malloc(sizeof(Page));
    init_fixed_len_page(page, 1000, 100);
    cout << *((int *)page->directory) << endl;
    cout << page << endl;
}
