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

    if (argc != 4) {
        std::cout << "Usage: " << argv[0];
        std::cout << " <csv_file> <heapfile> <page_size>" << std::endl;
        return 0;
    }

    // Get command line arguments
    std::ifstream csv_file(argv[1]);
    FILE *heapfile = fopen(argv[2], "w");
    int page_size = atoi(argv[3]);

    // Check for I/O errors
    if (heapfile == NULL) {
        std::cout << "Error opening heapfile" << std::endl;
        return -1;
    } else if (!csv_file.is_open()) {
        std::cout << "Unable to open CSV file" << std::endl;
        return -1;
    }

    std::string line;
    Page page;

    struct timeb t;
    ftime(&t);
    unsigned long start_time = t.time * 1000 + t.millitm;

    // Initialize a new page
    init_fixed_len_page(&page, page_size, RECORD_SIZE * V_SIZE);

    while (getline(csv_file, line)) {

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

        if (fixed_len_page_freeslots(&page) > 0) {

            // If the page is not full, add record to next free slot
            add_fixed_len_page(&page, &record);

        } else {

            // If the page is full, write current page to file,
            // and initialize a new page
            fwrite ((const void *) page.data, page_size, 1, heapfile);
            init_fixed_len_page(&page, page_size, RECORD_SIZE * V_SIZE);
            add_fixed_len_page(&page, &record);
        }
    }

    // Write the last page in memory to file
    fwrite ((const void *) page.data, page_size, 1, heapfile);
    ftime(&t);
    unsigned long stop_time = t.time * 1000 + t.millitm;
    std::cout << "TIME: " << stop_time - start_time << " milliseconds" << std::endl;
    
    fclose(heapfile);
    csv_file.close();

    return 0;
}