#include <iostream>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include "library.h"

///////////////////////////////// Part 2 /////////////////////////////////


int fixed_len_sizeof(Record *record){
    Record::const_iterator i;

    // The total bytes read from the record
    int total_bytes = 0;

    // Iterate through all elements in the vector
    for (i = record->begin(); i != record->end(); ++i){
        total_bytes += strlen(*i) * sizeof(char);
    }
    return total_bytes;
}


void fixed_len_write(Record *record, void *buf){
    Record::const_iterator i;

    // Keep track of the amount of bytes we have written
    int total_written = 0;
    int size;

    // Iterate through all elements in the vector
    for (i = record->begin(); i != record->end(); ++i){
        size = strlen(*i);

        // Cast the element as char* and copy the data to buffer
        char *ptr = (char *) *i;
        memcpy(buf, *i, size);

        // Update the buffer pointer using pointer arithmetic
        buf = static_cast<char*>(buf) + size;
    }
}


void fixed_len_read(void *buf, int size, Record *record){
    int total_read = 0;

    // Iterate through all the attributes in buffer
    while (total_read < size) {
        
        // Allocate a char array for temporarily storing each attribute 
        char *v = (char *) malloc(V_SIZE + 1);

        // Copy the attribute to the char array, then push to record
        memcpy(v, buf, V_SIZE);
        v[V_SIZE] = '\0';
        record->push_back((V) v);

        buf = static_cast<char*>(buf) + V_SIZE;
        total_read += V_SIZE;
    }
}


///////////////////////////////// Part 3 /////////////////////////////////


void init_fixed_len_page(Page *page, int page_size, int slot_size){
    page->data = calloc(page_size, 1);
    page->page_size = page_size;
    page->slot_size = slot_size;
}


int fixed_len_page_capacity(Page *page){
    int capacity = page->page_size / page->slot_size;
    return capacity;
}


int fixed_len_page_freeslots(Page *page){
    int capacity = fixed_len_page_capacity(page);
    int used_slots = 0;

    // Iterate through all slots and check if record exists
    for ( int i = 0; i < capacity; ++i) {
        int offset = i * page->slot_size;
        int size = strlen(static_cast<char*> (page->data) + offset);
        used_slots += (size > 0)? 1: 0;
    }

    return capacity - used_slots;
}

int add_fixed_len_page(Page *page, Record *r){
    int free_slots = fixed_len_page_freeslots(page);
    if (free_slots < 1) {
        return -1;
    }

    int capacity = fixed_len_page_capacity(page);
    for ( int i = 0; i < capacity; ++i) {

        // Check if data exists at the current slot
        int offset = i * (page->slot_size);
        char* data = static_cast<char*> (page->data) + offset;

        if (strlen(data) == 0) {

            // Serialize the record into the page
            fixed_len_write(r, (void *) data);

            return i;
        }
    }

    return -1;
}


void write_fixed_len_page(Page *page, int slot, Record *r){

    // Serialize the record into a buffer
    int r_size = fixed_len_sizeof(r);
    void* buf = malloc(r_size);
    fixed_len_write(r, buf);

    // Find pointer to the given slot and write from buffer to page
    int offset = slot * page->slot_size;
    char* data = static_cast<char*> (page->data) + offset;
    memcpy(data, buf, page->slot_size);
}


void read_fixed_len_page(Page *page, int slot, Record *r){

    // Find the pointer to the slot
    int offset = slot * page->slot_size;
    char* data = static_cast<char*> (page->data) + offset;

    // Copy the record to buffer, then write data from buffer to record
    void* buf = calloc(page->slot_size, 1);
    memcpy(buf, data, page->slot_size);
    fixed_len_read(buf, page->slot_size, r);
    free(buf);
}


void init_heapfile(Heapfile *heapfile, int page_size, FILE *file){
    heapfile->page_size = page_size;
    heapfile->file_ptr = file;
}


PageID alloc_page(Heapfile *heapfile){
    void *empty_page = calloc(heapfile->page_size, 1);

    PageID offset = 0;

    fseek(heapfile->file_ptr, 0, SEEK_SET);

    // Find the offset to the end of the file
    while (fgetc(heapfile->file_ptr) != EOF) {
        fseek(heapfile->file_ptr, heapfile->page_size, SEEK_CUR);
        // offset += heapfile->page_size;
        offset += 1;
    }

    fwrite(empty_page, heapfile->page_size, 1, heapfile->file_ptr);
    free(empty_page);
    return (PageID) offset; 
}


void read_page(Heapfile *heapfile, PageID pid, Page *page){
    // Set to the page with pid
    int offset = heapfile->page_size * pid;

    fseek(heapfile->file_ptr, offset, SEEK_SET);

    // Read data from heap file to page
    fread(page->data, heapfile->page_size, 1, heapfile->file_ptr);
}


void write_page(Page *page, Heapfile *heapfile, PageID pid){

    // Set to the page with pid
    int offset = heapfile->page_size * pid;
    fseek(heapfile->file_ptr, offset, SEEK_SET);

    // Write data from page to heap file
    fwrite(page->data, heapfile->page_size, 1, heapfile->file_ptr);
}


RecordIterator::RecordIterator(Heapfile *heapfile){
    heap_file = heapfile;
    record_id.page_id = 0;
    record_id.slot = 0;
    fseek(heapfile->file_ptr, 0, SEEK_SET);
    init_fixed_len_page(&page, heap_file->page_size, RECORD_SIZE*V_SIZE);
    read_page(heap_file, record_id.page_id, &page);
}

Record RecordIterator::next() {
    Record record;
    read_fixed_len_page(&page, record_id.slot++, &record);
    // ++record_id.slot;
    return record;
}

bool RecordIterator::hasNext() {

    int freespace = fixed_len_page_freeslots(&page);
    int capacity = fixed_len_page_capacity(&page);

    // check if we have already scan the entire page, if yes go load a new page, else keep reading from this page
    if (capacity - record_id.slot == freespace){

        // check if offset would exceed file boundary
        ++record_id.page_id;
        int offset = heap_file->page_size * record_id.page_id;
        fseek(heap_file->file_ptr, offset, SEEK_SET);
        long next_page_position = ftell(heap_file->file_ptr);
        fseek(heap_file->file_ptr, -1, SEEK_END);
        long end_of_file_position = ftell(heap_file->file_ptr);
        if(next_page_position > end_of_file_position)
            return false;

        read_page(heap_file, record_id.page_id, &page);

        // totally empty page which means we should stop reading, don't waste time checking following pages which are full of NULL
        if (strlen((char *) page.data) == 0) {
            return false;
        }

        record_id.slot = 0;
        
    }
        
    return true;
}