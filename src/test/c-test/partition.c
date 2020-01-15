#include <define.h>
#include "table.h"

int main(int argc, char *argv[]){

    int64_t pivot;
    table_t table; 
    uint64_t size = 2 * DEFAULT_TABLE_SIZE, i;
    header_t header = {(field_t[]){{"Primary Key", sizeof(uint64_t)}, {"Row_id", sizeof(uint64_t)}}, 2};

    // get table size from cmd (optional)
    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-s") == 0 && i < argc - 1)
            size = atoi(argv[i + 1]);
    }

    // initialize, randomize, set row_id & display the input table
    table_init(&table, size, "Random Table: R", &header);
    table_randomize(&table);
    table_by_row(&table);
    table_print(&table);

    // partition table & display result
    uint64_t start = size/4, end = 3 * (size/4);
    fprintf(stdout, "\n\n\e[1;4mPartition\n\e[0m");
    header_print(&header);
    pivot = table_partition(&table, &(tuple_t){start, end});
    for(i = 0; i < table.size; i++){
        if(i >= start && i < end)
            fprintf(stdout, "\e[1;32m");
        if(i == pivot)
            fprintf(stdout, "\e[7m");
        tuple_print(table.tuples + i);
        fprintf(stdout, "\e[0m");
    }

    // test result
    if(pivot < start || pivot > end){
        fprintf(stdout, "\e[1;31mError\e[0m: Pivot %ld is not within [%lu, %lu)\n", pivot, start, end);
        table_free(&table);
        return 1;
    }

    for(i = start; i < end; i++){

        // smaller elements must come before the chosen one
        if(i < pivot && table.tuples[i].key >= table.tuples[pivot].key){
            fprintf(stdout, "\e[1;31mError\e[0m at index %lu -> %lu >= %lu)\n", i, table.tuples[i].key, table.tuples[pivot].key);
            break;
        }
        // greater than or equal must come right after
        else if(i > pivot && table.tuples[i].key < table.tuples[pivot].key){
            fprintf(stdout, "\e[1;31mError\e[0m at index %lu -> %lu < %lu)\n", i, table.tuples[i].key, table.tuples[pivot].key);
            break;
        }
    }
    if(i == end)
        fprintf(stdout, "\e[1;32mSuccess\e[0m\n");
    table_free(&table);

    return 0;
}