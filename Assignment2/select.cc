#include <iostream>
#include <string.h>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <sys/timeb.h>
#include "library.h"

int main(int argc, char *argv[]){
	if (argc != 6) {
        std::cout << "Usage: " << argv[0];
        std::cout << " <heapfile> <attribute_id> <start> <end> <page_size>";
        std::cout << std::endl;
        return 0;
    }

    FILE *heapfile; 
    heapfile = fopen(argv[1], "r+");

    // Check for I/O errors
    if (heapfile == NULL) {
        return -1;
    }
    fseek(heapfile, 0, SEEK_SET);

    unsigned int attribute_id = (unsigned int) atoi(argv[2]);
    std::string start = std::string(argv[3]);
    std::string end = std::string(argv[4]);
    int page_size = atoi(argv[5]);
    int slot_size = RECORD_SIZE * V_SIZE;


    Page page;
    int i;

    // Start the timer
    struct timeb t;
    ftime(&t);
    unsigned long start_time = t.time * 1000 + t.millitm;


    init_fixed_len_page(&page, page_size, slot_size);

    // Read all pages into memory
    while (fread(page.data, page_size, 1, heapfile) > 0){

    	int capacity = fixed_len_page_capacity(&page);

        // Check if record satisfies WHERE clause
    	for (i = 0; i < capacity; ++i) {
    		Record record;
    		read_fixed_len_page(&page, i, &record);
    		std::string attribute = std::string(record[attribute_id]);
    		if (attribute >= start && attribute <= end) {
    			std::cout << attribute.substr(1, 5) << std::endl;
    		}
    	}
    }

    fclose(heapfile);

    // Stop the timer
    ftime(&t);
    unsigned long stop_time = t.time * 1000 + t.millitm;
    std::cout << "TIME: " << stop_time - start_time << " milliseconds" << std::endl;
	return 0;
}