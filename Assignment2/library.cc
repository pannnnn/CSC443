#include <vector>
typedef const char* V;
typedef std::vector<V> Record;

#define NUM_ATTRIBUTE 100
#define SIZE_ATTRIBUTE 10

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record)
{
    auto lambda = [&](int result, const char* current){ result + std::strlen(current); };
    return std::accumulate(record->begin(), record->end(), 0, lambda);
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf)
{
    for (int i = 0; i < record->size(); i++){
        // copy string at vector into buffer one by one
        int string_length = std::strlen(record->at(i));
        int position = (i * string_length);
        std::memcpy((char*) buf + position, record->at(i), string_length);
    }
};


/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record)
{
    // Requests that the vector capacity be at least enough to contain 100 elements.
    record->reserve(NUM_ATTRIBUTE);
    for (int i = 0; i < NUM_ATTRIBUTE; i++) {
        char *string_holder = (char*) malloc(SIZE_ATTRIBUTE+1);
        memcpy(string_holder, (char*)buf + SIZE_ATTRIBUTE * i, SIZE_ATTRIBUTE);
        string_holder[SIZE_ATTRIBUTE] = '\0';
        record->push_back(string_holder);
    }
}