#include <iostream>
#include <string.h>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include "library.h"

int main(int argc, char *argv[]){

    if (argc != 7) {
        std::cout << "Usage: " << argv[0];
        std::cout << " <heapfile> <record_id.page_id> <record_id.slot> ";
        std::cout << "<attribute_id> <new_value> <page_size>" << std::endl;
        return 0;
    }

    // Get command line arguments
    FILE *heapfile; 
    heapfile = fopen(argv[1], "r+");
    int page_id = atoi(argv[2]);
    int slot = atoi(argv[3]);
    int attribute_id = atoi(argv[4]);
    char *new_value = argv[5];
    int page_size = atoi(argv[6]);

    // Check for I/O errors
    if (heapfile == NULL) {
        return -1;
    }

    // Read the page into memory
    Page page;
    init_fixed_len_page(&page, page_size, RECORD_SIZE * V_SIZE);
    fseek(heapfile, page_size * page_id, SEEK_SET);
    
    if (fread(page.data, page_size, 1, heapfile) == 0){
    	std::cout << "Failed to find page with given page_id" << std::endl;
    	return 0;
    }


    if (strlen(new_value) != V_SIZE) {
    	std::cout << "The length of new_value must be 10" << std::endl;
    }

    // Find the record in the page
    Record record;
    if (slot >= fixed_len_page_capacity(&page)){
    	std::cout << "Slot out of range for given page_size" << std::endl;
    	return 0;
    }

    // Update the attribute
    read_fixed_len_page(&page, slot, &record);
    const int i = (const int) slot;
    V v = new_value;
    record[i] = v;

    // Write record back to page
    write_fixed_len_page(&page, slot, &record);

    // Write page back to heapfile
    fseek(heapfile, page_size * page_id, SEEK_SET);
    fwrite(page.data, page_size, 1, heapfile);

    return 0;
}