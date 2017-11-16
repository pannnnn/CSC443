#include <iostream>
#include <string.h>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include "library.h"

int main(int argc, char *argv[]){

    if (argc != 4) {
        std::cout << "Usage: " << argv[0];
        std::cout << " <heapfile> <csv_file> <page_size>" << std::endl;
        return 0;
    }

    // Get command line arguments
    FILE *file_ptr = fopen(argv[1], "r+");
    std::ifstream csv_file(argv[2]);
    int page_size = atoi(argv[2]);

    // Check for I/O errors
    if (file_ptr == NULL) {
        std::cout << "Error opening heapfile" << std::endl;
        return -1;
    } else if (!csv_file.is_open()) {
        std::cout << "Unable to open CSV file" << std::endl;
        return -1;
    }

    std::string line;
    Page page;
    int slot_size = RECORD_SIZE * V_SIZE;
    bool record_added;

    PageID page_id = 0;
    char *end;

    // Initialize a new page
    init_fixed_len_page(&page, page_size, slot_size);
    void *buffer = malloc(slot_size);
    
    fseek(file_ptr, 0, SEEK_SET);
    int offset = 0;
    while (getline(csv_file, line)) {
        record_added = 0;
        Record record;
        std::string attribute;
        std::stringstream ss(line);
        char *v;

        // Get all attributes for the record
        while (getline(ss, attribute, ',')){
            v = (char *) calloc(V_SIZE + 1, 1);
            std::strncpy(v, attribute.c_str(), V_SIZE);
            v[V_SIZE] = '\0';
            record.push_back((V) v);
        }

        // Try to find a free slot if EOF is not reached
        while(!feof(file_ptr)) {

            // Read a record into memory
            void *heap_slot = calloc(slot_size + 1, 1);
            fread(heap_slot, slot_size, 1, file_ptr);
            end = (char *) heap_slot + slot_size;
            end = '\0';

            // Check if the slot is free
            if (strlen((char *) heap_slot) == 0) {

                fixed_len_write(&record, buffer);

                // Insert record to the slot
                fseek(file_ptr, (-1) * slot_size, SEEK_CUR);
                fwrite(buffer, slot_size, 1, file_ptr);
                fseek(file_ptr, slot_size, SEEK_CUR);
                record_added = 1;
                break;
            }
        }
        
        if (feof(file_ptr) && (record_added == 0)) {

            if (fixed_len_page_freeslots(&page) > 0) {

                // If the page is not full, add record to next free slot
                add_fixed_len_page(&page, &record);

            } else {

                // If the page is full, write current page to Heapfile,
                // and initialize a new page
                fwrite ((const void *) page.data, page_size, 1, file_ptr);
                fseek(file_ptr, slot_size, SEEK_CUR);
                init_fixed_len_page(&page, page_size, RECORD_SIZE * V_SIZE);
                add_fixed_len_page(&page, &record);
            }
        }
    }

    if (fixed_len_page_freeslots(&page) > 0){

    	// Write the last page in memory to file
    	fwrite ((const void *) page.data, page_size, 1, file_ptr);
    }

    fclose(file_ptr);
    
    return 0;
}