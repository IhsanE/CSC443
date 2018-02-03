#include <stdio.h>
#include <math.h>
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
        char* temp_record_buff = new char[ATTR_SIZE + 1];
        memcpy(temp_record_buff, buff_reference, ATTR_SIZE);
        temp_record_buff[ATTR_SIZE] = '\0';
        (*record).push_back(temp_record_buff);
        buff_reference = (char *)buff_reference + ATTR_SIZE;
    }
}

void init_fixed_len_page(Page *page, int page_size, int slot_size) {
    free(page->data);
    page->data = malloc(page_size);
    page->page_size = page_size;
    page->slot_size = slot_size;

    int num_slots = ceil((float)page_size/(float)slot_size);

    page->num_records = num_slots - 1;

    void *directory_end = (page->data + sizeof(char)*page_size);
    // init directory slot to all 0
    memset(page->data, 0, page_size);

    // init first int size bits to represent the number of slots in page
    *((int*)(directory_end - sizeof(int))) = page->num_records;
    page->directory = directory_end;
}


int fixed_len_page_capacity(Page *page) {
    return page->num_records;
}

int fixed_len_page_freeslots(Page *page) {
    // Track how many bits we've explored (max of number of page records)
    int count = 0;
    int num_used_slots = 0;
    // Iterate through the directory by char size bytes
    int char_count = 1;
    void *dir_start = page->directory;
    dir_start = (dir_start - sizeof(int));
    // Iterate through directory starting from free/occupied bits sector
    while (count < page->num_records) {
        char cur_char = *((char * )(dir_start - sizeof(char) * char_count));
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

int find_first_free_slot(Page *page) {
    // Track how many bits we've explored (max of number of page records)
    int count = 0;
    // Iterate through the directory by char size bytes
    int char_count = 1;
    void *dir_start = page->directory;
    dir_start = (dir_start - sizeof(int));
    // Iterate through directory starting from free/occupied bits sector
    while (count < page->num_records) {
        char cur_char = *((char *)(dir_start - sizeof(char) * char_count));
        int char_bits = 0;
        while (char_bits < sizeof(char) && count < page->num_records) {
            if (!(cur_char & 0x1)) return count;
            cur_char = cur_char / 2;
            char_bits++;
            count++;
        }
        char_count++;
    }
    return -1;
}

int mark_slot_dirty(Page *page, int slot_num) {
    // Track how many bits we've explored (max of number of page records)
    int count = 0;
    // Iterate through the directory by char size bytes
    int char_count = 1;
    void *dir_start = page->directory;
    dir_start = (dir_start - sizeof(int));
    // Iterate through directory starting from free/occupied bits sector
    while (count < page->num_records) {
        char cur_char = *((char *)(dir_start - sizeof(char) * char_count));
        int char_bits = 0;
        while (char_bits < sizeof(char) && count < page->num_records) {
            if (count == slot_num)  {
                *((char *)(dir_start - sizeof(char) * char_count)) |= 0x1;
                return 0;
            }
            cur_char = cur_char / 2;
            char_bits++;
            count++;
        }
        char_count++;
    }
    return -1;
}

int add_fixed_len_page(Page *page, Record *r) {
    if (fixed_len_page_freeslots(page) == 0) return -1;

    int free_slot = find_first_free_slot(page);

    write_fixed_len_page(page, free_slot, r);

    return free_slot;
}

void write_fixed_len_page(Page *page, int slot, Record *r) {
    void *slot_start = (page->data + page->slot_size*(slot));
    fixed_len_write(r, slot_start);
    mark_slot_dirty(page, slot);
}
void read_fixed_len_page(Page *page, int slot, Record *r) {
    void *slot_start = (page->data + page->slot_size*(slot));
    fixed_len_read(slot_start, page->slot_size, r);
}
