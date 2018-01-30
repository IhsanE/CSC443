#include <vector>

typedef const char* V;
typedef std::vector<V> Record;
const int ATTR_SIZE = 10;

int fixed_len_sizeof(Record *record);
void fixed_len_write(Record *record, void *buf);
void fixed_len_read(void *buf, int size, Record *record);
