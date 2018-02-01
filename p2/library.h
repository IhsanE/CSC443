#include <vector>

typedef const char* V;
typedef std::vector<V> Record;
typedef struct {
    void *data;
    void *directory;
    int page_size;
    int slot_size;
    int num_records;
} Page;
const int ATTR_SIZE = 10;

int fixed_len_sizeof(Record *record);
void fixed_len_write(Record *record, void *buf);
void fixed_len_read(void *buf, int size, Record *record);
void init_fixed_len_page(Page *page, int page_size, int slot_size);
int fixed_len_page_capacity(Page *page);
int fixed_len_page_freeslots(Page *page);
int add_fixed_len_page(Page *page, Record *r);
void write_fixed_len_page(Page *page, int slot, Record *r);
void read_fixed_len_page(Page *page, int slot, Record *r);
