#include <iostream>
#include <fstream>
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <sys/stat.h>
#include <sstream>
#include <cstring>
#include "library.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 4) {
        cout << "Usage: csv2colstore <csv_file> <colstore_name> <page_size>" << endl;
        exit(EXIT_FAILURE);
    }

    ifstream csv_file(argv[1]);
    if (!csv_file.is_open()) {
        cout << "Error: unable to open the file " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }

    // check if directory exists, if not, create one
    char *colstore_name = argv[2];
    struct stat st = {0};
    if (stat(colstore_name, &st) == -1) {
        mkdir(colstore_name, 0700);
    }

    int page_size = atoi(argv[3]);
    // int record_size = V_SIZE * RECORD_SIZE;

    // start recording the time
    clock_t begin_time = clock();

    // read csv file into records
    vector<Record> records;
    string line;
    while (getline(csv_file, line)) {

        Record record;
        string attribute;
        stringstream ss(line);
        char *v;

        // Get all attributes for the record
        while (getline(ss, attribute, ',')){
            v = (char *) calloc(V_SIZE + 1, 1);
            strncpy(v, attribute.c_str(), V_SIZE);
            v[V_SIZE] = '\0';
            record.push_back((V) v);
        }
        records.push_back(record);
    }
    csv_file.close();

    // store records as files based on attribute under columnstore directory
    for (int i = 0; i < RECORD_SIZE; ++i) {
        // construct col_store heap file
        ostringstream col_store_heap_file_path_stream;
        col_store_heap_file_path_stream << colstore_name << "/" << i;
        const char* col_store_heap_file_path = col_store_heap_file_path_stream.str().c_str();

        FILE *col_store_heap_file_fp = fopen(col_store_heap_file_path, "w+");
        if (!col_store_heap_file_fp) {
            cout << "Error: unable to open the file" << col_store_heap_file_path << endl;
            exit(EXIT_FAILURE);
        }

        Heapfile heap_file;
        init_heapfile(&heap_file, page_size, col_store_heap_file_fp);

        Page page;
        init_fixed_len_page(&page, page_size, V_SIZE * RECORD_SIZE);

        Record col_store_record;
        int slot_index;
        PageID page_id;
        
        for (int j = 0; j < records.size(); ++j){
            // if (j == 300)
            //     break;

            col_store_record.push_back(records.at(j).at(i));

            // write to page if record is full
            if (col_store_record.size() == RECORD_SIZE) {
                slot_index = add_fixed_len_page(&page, &col_store_record);
                // no available slot, allocate new page
                if (slot_index == -1) {
                    page_id = alloc_page(&heap_file);
                    // cout << "Writing the page ... at index " << j << endl;
                    write_page(&page, &heap_file, page_id);
                    init_fixed_len_page(&page, page_size, V_SIZE * RECORD_SIZE);
                    slot_index = add_fixed_len_page(&page, &col_store_record);
                } else {
                    write_fixed_len_page(&page, slot_index, &col_store_record);
                }
                col_store_record.clear();
            }
        }

        // cout << "Before last write" << endl;
        
        if (col_store_record.size() == 0) {
            page_id = alloc_page(&heap_file);
            // cout << "Writing the last page ... at index index " << i << endl;
            write_page(&page, &heap_file, page_id);
            // fseek(heap_file.file_ptr, 0, SEEK_END);
            // std::cout << "Current position is " << ftell(heap_file.file_ptr) << std::endl;
        } else {
            // if no available slot in page, allocate a new page
            slot_index = add_fixed_len_page(&page, &col_store_record);
            if (slot_index == -1) {
                page_id = alloc_page(&heap_file);
                // cout << "Writing second last page ... at index " << i << endl;
                write_page(&page, &heap_file, page_id);
                init_fixed_len_page(&page, page_size, V_SIZE * RECORD_SIZE);
                // should be the first record in the page
                add_fixed_len_page(&page, &col_store_record);
            } else {
                write_fixed_len_page(&page, slot_index, &col_store_record);
            }
            page_id = alloc_page(&heap_file);
            // cout << "Writing the last page ... at index " << i << endl;
            write_page(&page, &heap_file, page_id);
        }

        fclose(col_store_heap_file_fp);
    }

    // calculate time elapsed and print it out
    clock_t end_time = clock();
    double time_elapsed = double(end_time - begin_time) / CLOCKS_PER_SEC;
    std::cout << "TIME: " << time_elapsed << " seconds" << endl;
}