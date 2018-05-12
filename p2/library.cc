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
    page->data = malloc(page_size);
    page->page_size = page_size;
    page->slot_size = slot_size;

    int num_slots = ceil((float)page_size/(float)slot_size);

    page->num_records = num_slots - 1;

    void *directory_end = ((char *)page->data + sizeof(char)*page_size);
    // init directory slot to all 0
    memset(page->data, 0, page_size);

    // init first int size bits to represent the number of slots in page
    *((int*)((int *)directory_end - sizeof(int))) = page->num_records;
    page->directory = directory_end;
}

int get_directory_offset(Heapfile *heapfile) {
    int num_slots = ceil((float)heapfile->page_size/(float)(sizeof(int)*3));
    return num_slots - 1;
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
    dir_start = ((char *)dir_start - sizeof(int));
    // Iterate through directory starting from free/occupied bits sector
    while (count < page->num_records) {
        char cur_char = *((char * )((char *)dir_start - sizeof(char) * char_count));
        unsigned int char_bits = 0;
        while (char_bits < sizeof(char) && count < page->num_records) {
            if ((cur_char & 0x1) == 1) num_used_slots++;
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
    dir_start = ((char *)dir_start - sizeof(int));
    // Iterate through directory starting from free/occupied bits sector
    while (count < page->num_records) {
        char cur_char = *((char *)((char *)dir_start - sizeof(char) * char_count));
        unsigned int char_bits = 0;
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
    dir_start = ((char *)dir_start - sizeof(int));
    // Iterate through directory starting from free/occupied bits sector
    while (count < page->num_records) {
        char cur_char = *((char *)((char *)dir_start - sizeof(char) * char_count));
        unsigned int char_bits = 0;
        while (char_bits < sizeof(char) && count < page->num_records) {
            if (count == slot_num)  {
                *((char *)((char *)dir_start - sizeof(char) * char_count)) |= 0x1;
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

int mark_slot_clean(Page *page, int slot_num) {
    // Track how many bits we've explored (max of number of page records)
    int count = 0;
    // Iterate through the directory by char size bytes
    int char_count = 1;
    void *dir_start = page->directory;
    dir_start = ((char *)dir_start - sizeof(int));
    // Iterate through directory starting from free/occupied bits sector
    while (count < page->num_records) {
        char cur_char = *((char *)((char *)dir_start - sizeof(char) * char_count));
        unsigned int char_bits = 0;
        while (char_bits < sizeof(char) && count < page->num_records) {
            if (count == slot_num)  {
                *((char *)((char *)dir_start - sizeof(char) * char_count)) &= 0x0;
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

int is_slot_free(Page *page, int slot_num) {
    // Track how many bits we've explored (max of number of page records)
    int count = 0;
    // Iterate through the directory by char size bytes
    int char_count = 1;
    void *dir_start = page->directory;
    dir_start = ((char *)dir_start - sizeof(int));
    // Iterate through directory starting from free/occupied bits sector
    while (count < page->num_records) {
        char cur_char = *((char *)((char *)dir_start - sizeof(char) * char_count));
        unsigned int char_bits = 0;
        while (char_bits < sizeof(char) && count < page->num_records) {
            if (count == slot_num)  {
                return *((char *)((char *)dir_start - sizeof(char) * char_count)) ^ 0x1;
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
    void *slot_start = ((char *)page->data + page->slot_size*(slot));
    fixed_len_write(r, slot_start);
    mark_slot_dirty(page, slot);
}
void read_fixed_len_page(Page *page, int slot, Record *r) {
    void *slot_start = ((char *)page->data + page->slot_size*(slot));
    fixed_len_read(slot_start, page->slot_size, r);
}

// It is assumed the start/end indices reflect how far along they are in the heap file (offset included)
void init_directory_heap_page (Page *page, int start, int end, int dir_num, int page_size) {
    char *directory_data = (char *) page->data;
    for (int i = start; i < end; i++) {
        directory_data = directory_data + (sizeof(int)*3);
        // page id
        memcpy(directory_data, &i, sizeof(int));
        // offset
        int offset = i*page_size*dir_num;
        memcpy((directory_data + sizeof(int)), &offset, sizeof(int));
        // freespace
        memcpy((directory_data + sizeof(int)*2), &page_size, sizeof(int));
    }
}

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file){
    heapfile->file_ptr = file;
    heapfile->page_size = page_size;
    fseek(heapfile->file_ptr, 0, SEEK_SET);

    Page *page = (Page *)malloc(sizeof(Page));
    // Each slot is for ID(p): (page_offset, free_space)
    // Each are ints, so we need to store 3 ints per page
    init_fixed_len_page(page, page_size, sizeof(int)*3);
    init_directory_heap_page(page, 0, page->num_records-10, 0, page_size);

    // Initialize the first 8bytes of the file to be a pointer to the next heap directory
    // Add our page directory created above to the file
    uint8_t pointer_to_next_directory = 0;
    fwrite(&pointer_to_next_directory, sizeof(uint8_t), 1, heapfile->file_ptr);
    fwrite(page->data, page_size, 1, heapfile->file_ptr);
    fseek(heapfile->file_ptr, 0, SEEK_SET);

    free(page->data);
    free(page);
}

PageID alloc_page(Heapfile *heapfile) {
    fseek(heapfile->file_ptr, 0, SEEK_SET);
    int num_dirs;
    fread(&num_dirs, sizeof(uint8_t), 1, heapfile->file_ptr);

    int cur_dir = 0;

    Page *directory_page = (Page *)malloc(sizeof(Page));
    init_fixed_len_page(directory_page, heapfile->page_size, sizeof(int)*3);
    fseek(heapfile->file_ptr, (((int) get_directory_offset(heapfile))*cur_dir)*heapfile->page_size + sizeof(uint8_t), SEEK_SET);
    do {
        fread(directory_page->data, heapfile->page_size, 1, heapfile->file_ptr);
        char *directory_data = (char *) directory_page->data;
        for (int i = 0; i < directory_page->num_records - 10; i++) {
            directory_data = directory_data + (sizeof(int)*3);
            PageID pid = *((PageID *) directory_data);
            int freespace = *((int *) (directory_data + sizeof(int)*2));
            if (freespace == heapfile->page_size) {
                return pid;
            }
        }
    } while (cur_dir < num_dirs);
    return -1;
}

void read_page(Heapfile *heapfile, PageID pid, Page *page) {
    int offset = pid/(((int) get_directory_offset(heapfile)) - 10);
    fseek(heapfile->file_ptr, (pid + 1 + offset)*heapfile->page_size + sizeof(uint8_t), SEEK_SET);
    fread(page->data, heapfile->page_size, 1, heapfile->file_ptr);
}

void write_page(Page *page, Heapfile *heapfile, PageID pid) {
   fseek(heapfile->file_ptr, 0, SEEK_SET);
   void *directory_pointer = malloc(sizeof(uint8_t));
   fread(directory_pointer, sizeof(uint8_t), 1, heapfile->file_ptr);

   // The offset is representative of which "directory page" we refer to
   int offset = pid/(((int) get_directory_offset(heapfile)) - 10);
   fseek(heapfile->file_ptr, (pid + 1 + offset)*heapfile->page_size + sizeof(uint8_t), SEEK_SET);
   fwrite(page->data, heapfile->page_size, 1, heapfile->file_ptr);

   // Go to the start of the directory (directory pages are spaced out by number of records possible to store)
   Page *directory_page = (Page *)malloc(sizeof(Page));
   init_fixed_len_page(directory_page, heapfile->page_size, sizeof(int)*3);
   fseek(heapfile->file_ptr, (((int) get_directory_offset(heapfile))*offset)*heapfile->page_size + sizeof(uint8_t), SEEK_SET);
   if (offset <= *((uint8_t *) directory_pointer)) {
      fread(directory_page->data, heapfile->page_size, 1, heapfile->file_ptr);
   } else {
      fseek(heapfile->file_ptr, 0, SEEK_SET);
      init_directory_heap_page(directory_page, 0, page->num_records-10, offset, heapfile->page_size);
      fwrite(&offset, sizeof(uint8_t), 1, heapfile->file_ptr);
   }
   char *directory_data = (char *) directory_page->data;
   for (int i = 0; i < directory_page->num_records - 10; i++) {
      directory_data = directory_data + (sizeof(int)*3);
      PageID cur_pid = *((PageID *) directory_data);
      if (cur_pid == pid) {
         int empty = 0;
         memcpy((directory_data + sizeof(int)*2), &empty, sizeof(int));
         fseek(heapfile->file_ptr, (((int) get_directory_offset(heapfile))*offset)*heapfile->page_size + sizeof(uint8_t), SEEK_SET);
         fwrite(directory_page->data, heapfile->page_size, 1, heapfile->file_ptr);
         goto end;
      }
   }
   end:;
}

RecordIterator::RecordIterator (Heapfile *init_heapfile) {
   heapfile = init_heapfile;
   cur_record_slot = 0;
   cur_page_slot = 0;
   fseek(heapfile->file_ptr, 0, SEEK_SET);
   fread(&num_dirs, sizeof(uint8_t), 1, heapfile->file_ptr);
   cur_record = Record();
   cur_page = (Page *)malloc(sizeof(Page));
   dir_page = (Page *)malloc(sizeof(Page));
   init_fixed_len_page(cur_page, heapfile->page_size, NUM_ATTR*ATTR_SIZE);
   init_fixed_len_page(dir_page, heapfile->page_size, sizeof(int)*3);
   fseek(heapfile->file_ptr, sizeof(uint8_t), SEEK_SET);
   fread(dir_page->data, heapfile->page_size, 1, heapfile->file_ptr);
   fread(cur_page->data, heapfile->page_size, 1, heapfile->file_ptr);
}

BoostedRecord RecordIterator::next() {
   // Fast forward to current page start
   uint8_t cur_dir = cur_page_slot/(((int) get_directory_offset(heapfile)) - 10);
   do {
      int offset = cur_page_slot/(((int) get_directory_offset(heapfile)) - 10);
      fseek(heapfile->file_ptr, (cur_page_slot + 1 + offset)*heapfile->page_size + sizeof(uint8_t), SEEK_SET);
      cur_record = Record();
      // Iterate through the pages, and then within pages, through the records
      int i = 0;
      while (i < dir_page->num_records - 10) {
         for (int i_rec_slot = cur_record_slot; i_rec_slot < cur_page->num_records; i_rec_slot++) {
            // means the slot is used
            if (!is_slot_free(cur_page, i_rec_slot)) {
               cur_record_slot = i_rec_slot;
               fixed_len_read((char *) cur_page->data + (sizeof(char) * NUM_ATTR * ATTR_SIZE)*cur_record_slot, NUM_ATTR * ATTR_SIZE, &cur_record);
               cur_record_slot++;
               BoostedRecord *boosted_record  = (BoostedRecord *)malloc(sizeof(BoostedRecord));
               RecordID *rid = (RecordID*)malloc(sizeof(RecordID));
               rid->page_id = cur_page_slot;
               rid->slot = i_rec_slot;
               boosted_record->record = &cur_record;
               boosted_record->id = rid;
               return *boosted_record;
            }
         }
         cur_page_slot++;
         cur_record_slot = 0;
         memset(cur_page->data, 0, heapfile->page_size);
         // At this point, the actual file_ptr is one page behind, so fast forward it by a page
         fseek(heapfile->file_ptr, sizeof(uint8_t) + heapfile->page_size + heapfile->page_size*cur_page_slot, SEEK_SET);
         fread(cur_page->data, heapfile->page_size, 1, heapfile->file_ptr);
         i++;
      }
      cur_dir++;
   } while(cur_dir <= num_dirs);
   // Null record as default return
   cur_record = Record();
   BoostedRecord *boosted_record  = (BoostedRecord *)malloc(sizeof(BoostedRecord));
   RecordID rid = RecordID();
   boosted_record->record = &cur_record;
   boosted_record->id = &rid;
   return *boosted_record;
}

bool RecordIterator::hasNext() {
   // Fast forward to current page start
   int ghost_cur_page_slot = cur_page_slot;
   int ghost_cur_record_slot = cur_record_slot;
   int cur_dir = ghost_cur_page_slot/(((int) get_directory_offset(heapfile)) - 10);
   do {
      int offset = ghost_cur_page_slot/(((int) get_directory_offset(heapfile)) - 10);
      fseek(heapfile->file_ptr, (ghost_cur_page_slot + 1 + offset)*heapfile->page_size + sizeof(uint8_t), SEEK_SET);
      cur_record = Record();
      // Iterate through the pages, and then within pages, through the records
      int i = 0;
      while (i < dir_page->num_records - 10) {
         for (int i_rec_slot = ghost_cur_record_slot; i_rec_slot < cur_page->num_records; i_rec_slot++) {
            // means the slot is used
            if (!is_slot_free(cur_page, i_rec_slot)) {
               return true;
            }
         }
         ghost_cur_page_slot++;
         ghost_cur_record_slot = 0;
         memset(cur_page->data, 0, heapfile->page_size);
         // At this point, the actual file_ptr is one page behind, so fast forward it by a page
         fseek(heapfile->file_ptr, sizeof(uint8_t) + heapfile->page_size + heapfile->page_size*ghost_cur_page_slot, SEEK_SET);
         fread(cur_page->data, heapfile->page_size, 1, heapfile->file_ptr);
         i++;
      }
      cur_dir++;
   } while(cur_dir <= num_dirs);
   cur_record = Record();
   return false;
}
