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
        std::cout << " <csv_file> <page_file> <page_size>" << std::endl;
        return 0;
    }

    // Get command line arguments
    std::ifstream csv_file(argv[1]);
    std::ofstream page_file(argv[2], std::ios::out | std::ios::binary);
    int page_size = atoi(argv[3]);

    std::string line;
    Page page;

    int num_records = 0;
    int num_pages = 0;

    // Start the time
    struct timeb t;
    ftime(&t);
    unsigned long start_time = t.time * 1000 + t.millitm;

    // Initialize a new page
    init_fixed_len_page(&page, page_size, RECORD_SIZE * V_SIZE);
    
    if (csv_file.is_open()) {

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
                page_file.write((const char *) &(page.page_size), sizeof(int));
                page_file.write((const char *) &(page.slot_size), sizeof(int));
                page_file.write((const char *) page.data, page.page_size);
                ++num_pages;
                init_fixed_len_page(&page, page_size, RECORD_SIZE * V_SIZE);
                add_fixed_len_page(&page, &record);
            }
            ++num_records;
        }

        // Write the last page in memory to file
        page_file.write((const char *) &(page.page_size), sizeof(int));
        page_file.write((const char *) &(page.slot_size), sizeof(int));
        page_file.write((const char *) page.data, page.page_size);
        ++num_pages;

        // Stop the timer
        ftime(&t);
        unsigned long stop_time = t.time * 1000 + t.millitm;

        page_file.close();
        csv_file.close();

        std::cout << "NUMBER OF RECORDS: " << num_records << std::endl;
        std::cout << "NUMBER OF PAGES: " << num_pages << std::endl;
        std::cout << "TIME: " << stop_time - start_time << " milliseconds\n";

    } else {
        std::cout << "Unable to open CSV file" << std::endl;;
    }

    return 0;
}