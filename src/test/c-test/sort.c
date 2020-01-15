#include <define.h>
#include <table.h>
#define SORT_DEFAULT 0

int main(int argc, char *argv[]){

    uint8_t i, method = SORT_DEFAULT;
    table_t table, table_q; 
    uint64_t size = DEFAULT_TABLE_SIZE;
    header_t header = {(field_t[]){{"Primary Key", sizeof(uint64_t)}, {"Row_id", sizeof(uint64_t)}}, 2};

    // get table size from cmd (optional)
    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-s") == 0 && i < argc - 1)
            size = atoi(argv[i + 1]);
        else if(strcmp(argv[i], "-m") == 0 && i < argc - 1)
            method = atoi(argv[i + 1]);
    }

    // initialize, randomize, set row_id & display input table
    table_init(&table, size, "Random Table: R", &header);
    table_randomize(&table);
    table_by_row(&table);
    table_print(&table);

    // create a clone of the table to be sorted with a library sorting method
    table_init(&table_q, size, "Quick-sorted Table: Q", &header);
    table_clone(&table, &table_q);

    // sort clone table with stdlib.qsort & check whether these tables are the same
    qsort(table_q.tuples, table_q.next, sizeof(tuple_t), tuple_compare);
    table_print(&table_q);

    // sort the un-ordered table using the requested method, result goes into the ordered one
    if(method) 
        // quick-sort in place!
        table_quick_sort(&table, &(tuple_t){0, table.next}); // quick-sort
    else
        // radix-sort in place!
        table_sort_wrapper(&table); // radix-sort
    
    if(table_equals(&table, &table_q) == 1)
        fprintf(stdout, "\n\e[1;32mSuccess\e[0m: sort method %d succeeded\n", method);
    else
        fprintf(stdout, "\n\e[1;31mFailure\e[0m: sort method %d failed\n", method);

    // display sorted table
    table_print(&table);

    // free all resources
    table_free(&table);
    table_free(&table_q);
    return 0;
}