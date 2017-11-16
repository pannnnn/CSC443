#include <iostream>
#include <fstream>
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <sstream>
#include <cstring>
#include "library.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 6) {
        cout << "Usage: select2  <colstore_name> <attribute_id> <start> <end> <page_size>" << endl;
        exit(EXIT_FAILURE);
    }

    char *colstore_name = argv[1];

    int attribute_id = atoi(argv[2]);
    if (attribute_id < 0 || attribute_id >= RECORD_SIZE) {
        cout << "Error: attribute_id: [" << attribute_id << "] is no in 0 - 99\n";
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

    // char *start = argv[3];
    // char *end = argv[4];

    std::string start = std::string(argv[3]);
    std::string end = std::string(argv[4]);

    int page_size = atoi(argv[5]);
    Heapfile heap_file;
    init_heapfile(&heap_file, page_size, col_store_heap_file_fp);

    // start recording the time
    clock_t begin_time = clock();

    RecordIterator record_iterator(&heap_file);
    // Record cur_record, result_record;
    Record cur_record;
    Record::const_iterator i, j;

    // Iterate through heap file to get the record one at a time and compare with params given
    // int counter;
    while(record_iterator.hasNext()){
        cur_record = record_iterator.next();
        for (i = cur_record.begin(); i != cur_record.end(); ++i){
            // if(strncmp(*i, start, strlen(start)) >= 0 && strncmp(*i, end, strlen(end)) <= 0){
            //     result_record.push_back(*i);
            // }
            if (string(*i) >= start && string(*i) <= end) {
                // if(counter == 1002){
                //     counter = 0;
                //     break;
                // }
    			cout << string(*i).substr(1, 5) << endl;
                // cout << ++counter << endl;
    		}
        }
    }

    fclose(col_store_heap_file_fp);

    // return the substring from [1, 5]
    // char tmp[SUBSTRING_LENGTH+1];
    // tmp[SUBSTRING_LENGTH] = '\0';
    // for (j = result_record.begin(); j != result_record.end(); ++j){
    //     strncpy(tmp, *j, SUBSTRING_LENGTH);
    //     cout << tmp << endl;
    // }

    // calculate time elapsed and print it out
    clock_t end_time = clock();
    double time_elapsed = double(end_time - begin_time) / CLOCKS_PER_SEC;
    // std::cout << "NUMBER OF RECORDS: " << records.size() << endl;
    std::cout << "TIME: " << time_elapsed << " milliseconds" << endl;
}