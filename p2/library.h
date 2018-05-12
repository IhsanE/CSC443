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
typedef int PageID;
typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile;
typedef struct {
    int page_id;
    int slot;
} RecordID;
// This is purely used for scan in order to return the RecordID when printing
typedef struct {
    Record *record;
    RecordID *id;
} BoostedRecord;
const int ATTR_SIZE = 10;
const int NUM_ATTR = 100;

int fixed_len_sizeof(Record *record);
int find_first_free_slot(Page *page);
void fixed_len_write(Record *record, void *buf);
void fixed_len_read(void *buf, int size, Record *record);
void init_fixed_len_page(Page *page, int page_size, int slot_size);
int fixed_len_page_capacity(Page *page);
int fixed_len_page_freeslots(Page *page);
int add_fixed_len_page(Page *page, Record *r);
void write_fixed_len_page(Page *page, int slot, Record *r);
void read_fixed_len_page(Page *page, int slot, Record *r);
int is_slot_free(Page *page, int slot_num);
int mark_slot_clean(Page *page, int slot_num);
int mark_slot_dirty(Page *page, int slot_num);

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);
PageID alloc_page(Heapfile *heapfile);
void read_page(Heapfile *heapfile, PageID pid, Page *page);
void write_page(Page *page, Heapfile *heapfile, PageID pid);
class RecordIterator {
    public:
    Heapfile *heapfile;
    Record cur_record;
    int cur_record_slot;
    int cur_page_slot;
    uint8_t num_dirs;
    Page *cur_page;
    Page *dir_page;
    RecordIterator(Heapfile *init_heapfile);
    BoostedRecord next();
    bool hasNext();
};
