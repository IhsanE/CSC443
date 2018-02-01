#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "library.h"
using namespace std;

#include <limits.h>
char *chartobin ( unsigned char c )
{
        static char bin[CHAR_BIT + 1] = {0};
            int i;
                for( i = CHAR_BIT - 1; i >= 0; i-- )
                        {
                                    bin[i] = (c % 2) + '0';
                                            c /= 2;
                                                }
                   return bin;
}

int fixed_len_sizeof(Record *record){
    int size = 0;
    for (Record::iterator iter = (*record).begin(); iter != (*record).end(); iter++) {
        size += strlen(*iter);
    }
    return size;
}
void fixed_len_write(Record *record, void *buf){
    void *buff_reference = buf;
    // Append characters from record into a buffer
    for (Record::iterator iter = (*record).begin(); iter != (*record).end(); iter++) {
        size_t record_size = strlen(*iter);
        memcpy(buff_reference, *iter, record_size);
        buff_reference = (char *)buff_reference + record_size;
    }
}
void fixed_len_read(void *buf, int size, Record *record) {
    void *buff_reference = buf;
    // Create a temp char buffer to read bytes into
    // Add this buffer to our record vector
    for (int i = 0; i < size/ATTR_SIZE; i++) {
        char* temp_record_buff = new char[size];
        memcpy(temp_record_buff, buff_reference, ATTR_SIZE);
        (*record).push_back(temp_record_buff);
        buff_reference = (char *)buff_reference + ATTR_SIZE;
    }
}

void init_fixed_len_page(Page *page, int page_size, int slot_size) {
    page->data = malloc(page_size);
    page->page_size = page_size;
    page->slot_size = slot_size;

    int num_slots = page_size/slot_size;

    page->num_records = num_slots - 1;

    void *directory_start = (page->data + slot_size*(num_slots - 1));
    // init directory slot to all 0
    memset(directory_start, 0, slot_size);

    // init first int size bits to represent the number of slots in page
    *((int*)directory_start) = page->num_records;
    page->directory = directory_start;
}


int fixed_len_page_capacity(Page *page) {
    return page->num_records;
}
int fixed_len_page_freeslots(Page *page) {
    // Track how many bits we've explored (max of number of page records)
    int count = 0;
    int num_used_slots = 0;
    // Iterate through the directory by char size bytes
    int char_count = 0;
    void *dir_start = page->directory;
    dir_start = (dir_start + sizeof(int));
    // Iterate through directory starting from free/occupied bits sector
    while (count < page->num_records) {
        char cur_char = *((char * )(dir_start + sizeof(char) * char_count));
        /* cout << chartobin(cur_char) << endl; */
        int char_bits = 0;
        while (char_bits < sizeof(char) && count < page->num_records) {
            if (cur_char & 0x1 == 1) num_used_slots++;
            cur_char = cur_char / 2;
            char_bits++;
            count++;
        }
        char_count++;
    }
    return page->num_records - num_used_slots;
}
int add_fixed_len_page(Page *page, Record *r);
void write_fixed_len_page(Page *page, int slot, Record *r);
void read_fixed_len_page(Page *page, int slot, Record *r);
