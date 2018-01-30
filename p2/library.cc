#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "library.h"
using namespace std;


int fixed_len_sizeof(Record *record){
    int size = 0;
    for (Record::iterator iter = (*record).begin(); iter != (*record).end(); iter++) {
        size += strlen(*iter);
    }
    return size;
}
void fixed_len_write(Record *record, void *buf){
    void *buff_reference = buf;
    for (Record::iterator iter = (*record).begin(); iter != (*record).end(); iter++) {
        size_t record_size = strlen(*iter);
        memcpy(buff_reference, *iter, record_size);
        buff_reference = (char *)buff_reference + record_size;
    }
}
void fixed_len_read(void *buf, int size, Record *record) {

}

int main () {
    Record r = Record();
    char* char_array = "wtf";
    r.push_back(char_array);
    r.push_back("alsdkfjakldjfl");
    cout << "Original Record:" << endl;
    for (Record::iterator iter = (r).begin(); iter != (r).end(); iter++) {
        cout << *iter << endl;
    }
    int size = fixed_len_sizeof(&r);
    cout << "Below is the size of the vector\n";
    cout << size << endl;

    void* buffer = malloc(size);;
    Record r_read = Record();

    fixed_len_write(&r, buffer);
    cout << "Below is the written buffer (shouldnt be legible)\n";
    cout << buffer << endl;

    fixed_len_read(buffer, size, &r_read);
    cout << "Read In Record:" << endl;
    for (Record::iterator iter = (r_read).begin(); iter != (r_read).end(); iter++) {
        cout << *iter << endl;
    }
}
