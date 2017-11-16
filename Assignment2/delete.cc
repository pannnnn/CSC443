#include <iostream>
#include <string.h>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include "library.h"


int main(int argc, char *argv[]){

    if (argc != 5) {
        std::cout << "Usage: " << argv[0];
        std::cout << " <heapfile> <record_id.page_id> <record_id.slot>";
        std::cout << " <page_size>" << std::endl;
        return 0;
    }

    // Get command line arguments
    FILE *heapfile; 
    heapfile = fopen(argv[1], "r+");
    int page_id = atoi(argv[2]);
    int slot = atoi(argv[3]);
    int page_size = atoi(argv[4]);

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

    // Delete the record in specified slot
    void* buffer = calloc(RECORD_SIZE * V_SIZE, 1);
    int offset = slot * RECORD_SIZE * V_SIZE;
    char* data = static_cast<char*> (page.data) + offset;
    memcpy(data, buffer, RECORD_SIZE * V_SIZE);

    // Write page back to heapfile
    fseek(heapfile, page_size * page_id, SEEK_SET);
    fwrite(page.data, page_size, 1, heapfile);

    return 0;
}