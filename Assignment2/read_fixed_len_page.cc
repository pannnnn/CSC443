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

    if (argc != 3) {
        std::cout << "Usage: " << argv[0];
        std::cout << " <page_file> <page_size>" << std::endl;
        return 0;
    }

    // Get command line arguments
    std::ifstream page_file(argv[1], std::ios::in | std::ios::binary);
    int page_size = atoi(argv[2]);

    // Check if page file is opened for read
    if (!page_file.is_open()){
    	std::cout << "Unable to open page file" << std::endl;
    	return 0;
    }

    int slot_size;
    Page page;
    int page_read = 0;
    int file_page_size;

    // Start the timer
    struct timeb t;
    ftime(&t);
    unsigned long start_time = t.time * 1000 + t.millitm;

    while (!page_file.fail() && !page_file.bad()){
    	++page_read;

    	// Read metadata about current page
        page_file.read((char *) &file_page_size, sizeof(int));
    	page_file.read((char *) &slot_size, sizeof(int));

        // Checks if the page_size in page_file is the 
        // same as the page_size specified by the user input
        if (file_page_size != page_size) {
            std::cout << "Page size in the page file does not ";
            std::cout << "equal to the <page_size> given" << std::endl;
            return 0;
        }

    	// Read the entire page size to the page's data
    	init_fixed_len_page(&page, page_size, slot_size);
    	page_file.read((char *) page.data, page_size);

    	// Check if the end of file is reached
    	if (page_file.eof()){

            // Stop the timer
            ftime(&t);
            unsigned long stop_time = t.time * 1000 + t.millitm;
            std::cout << "TIME: " << stop_time - start_time << " milliseconds\n";
    		page_file.close();
    		return 0;
    	}

    	// std::cout << "\nRead Page: " << page_read << std::endl;
    	
    	// void *buffer = malloc(slot_size);
    	// char temp[11];
    	int i;
    	unsigned int j;

    	// Print to stdout or write to CSV file
    	for (i = 0; i < fixed_len_page_capacity(&page); ++i) {
    		Record record;
    		read_fixed_len_page(&page, i, &record);

            for (j = 0; j < record.size() - 1; ++j) {
                std::cout << record.at(j) << ',';
            }
            std::cout << record.at(j) << '\n';
    	}
    }



    std::cout << "Error Reading Page File" << std::endl;


    return -1;
}