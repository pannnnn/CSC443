#include <iostream>
#include <fstream>
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <sstream>
#include <cstring>
#include "library.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 7) {
        cout << "Usage: select3 <colstore_name> <attribute_id> <attribute_id_return> <start> <end> <page_size>" << endl;
        exit(EXIT_FAILURE);
    }

    char *colstore_name = argv[1];

    // need two files open in order to do parallel operation
    int attribute_id = atoi(argv[2]);
    if (attribute_id < 0 || attribute_id >= RECORD_SIZE) {
        cout << "Error: attribute_id: [" << attribute_id << "] is no in 0 - 99\n";
        exit(EXIT_FAILURE);
    }

    int attribute_id_return = atoi(argv[3]);
    if (attribute_id_return < 0 || attribute_id_return >= RECORD_SIZE) {
        cout << "Error: attribute_id_return: [" << attribute_id_return << "] is no in 0 - 99\n";
        exit(EXIT_FAILURE);
    }

    ostringstream col_store_heap_file_path_stream;
    col_store_heap_file_path_stream << colstore_name << "/" << attribute_id;
    const char* col_store_heap_file_path = col_store_heap_file_path_stream.str().c_str();

    FILE *col_store_heap_file_fp = fopen(col_store_heap_file_path, "r+");
    if (!col_store_heap_file_fp) {
        cout << "Error: unable to open the file" << col_store_heap_file_path << endl;
        exit(EXIT_FAILURE);
    }

    ostringstream col_store_heap_file_path_stream_return;
    col_store_heap_file_path_stream_return << colstore_name << "/" << attribute_id_return;
    const char* col_store_heap_file_path_return = col_store_heap_file_path_stream_return.str().c_str();

    FILE *col_store_heap_file_fp_return = fopen(col_store_heap_file_path_return, "r+");
    if (!col_store_heap_file_fp_return) {
        cout << "Error: unable to open the file" << col_store_heap_file_path_return << endl;
        exit(EXIT_FAILURE);
    }

    string start = string(argv[4]);
    string end = string(argv[5]);

    int page_size = atoi(argv[6]);
    Heapfile heap_file;
    Heapfile heap_file_return;
    init_heapfile(&heap_file, page_size, col_store_heap_file_fp);
    init_heapfile(&heap_file_return, page_size, col_store_heap_file_fp_return);


    // start recording the time
    clock_t begin_time = clock();
    
    // two iterator to go through two heap files, and they are consistent with each other
    RecordIterator record_iterator(&heap_file);
    RecordIterator record_iterator_return(&heap_file_return);
    Record cur_record, cur_record_return, result_record;
    Record::const_iterator i, j, k;

    // Iterate through two heap files to get the records one at a time and compare with params given
    while(record_iterator.hasNext() && record_iterator_return.hasNext()){
        cur_record = record_iterator.next();
        cur_record_return = record_iterator_return.next();
        for (i = cur_record.begin(), j = cur_record_return.begin(); i != cur_record.end(); ++i, ++j){
            if (string(*i) >= start && string(*i) <= end) {
    			cout << string(*j).substr(1, 5) << endl;
    		}
        }
    }

    fclose(col_store_heap_file_fp);
    fclose(col_store_heap_file_fp_return);

    // calculate time elapsed and print it out
    clock_t end_time = clock();
    double time_elapsed = double(end_time - begin_time) / CLOCKS_PER_SEC;
    std::cout << "TIME: " << time_elapsed << " seconds" << endl;
}