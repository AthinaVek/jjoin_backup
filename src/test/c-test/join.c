#include <define.h>
#include "harness.h"
#define SORT_DEFAULT 0
#define JOIN_TABLES 2

int main(int argc, char *argv[]){

    uint8_t i, j;
    table_t tables[JOIN_TABLES];
    list_t list;
    int data[JOIN_TABLES][DEFAULT_TABLE_SIZE] = {{111, 222, 222, 222, 333}, {111, 111, 111, 333, 333}};
    header_t header = {(field_t[]){{"Primary Key", sizeof(uint64_t)}, {"Row_id", sizeof(uint64_t)}}, 2};

    // initialize & display tables
    table_init(tables, DEFAULT_TABLE_SIZE, "Table A", &header);
    table_init(tables + 1, DEFAULT_TABLE_SIZE, "Table B", &header);

    for(i = 0; i < JOIN_TABLES; i++){
        for(j = 0; j < DEFAULT_TABLE_SIZE; j++)
            table_insert(tables + i, &(tuple_t){data[i][j], j + 1});
        table_print(tables + i);
    }

    // initialize result list
    list_init(&list, "Result", sizeof(table_t), table_init_wrap, table_print, table_free);

    // join tables, result goes into the list
    join(tables, tables + 1, &list);

    // display result
    list_print(&list);

    // free all resources
    for(i = 0; i < JOIN_TABLES; i++)
        table_free(tables + i);
    list_free(&list);
    return 0;
}