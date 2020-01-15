#include <define.h>
#include <table.h>

int main(){

    // create & display a random table
    table_t table;
    uint64_t sz = 5;
    header_t header = {(field_t[]){{"Primary Key", sizeof(uint64_t)}, {"Row_id", sizeof(uint64_t)}}, 2};

    // initialize table
    table_init(&table, sz, "Random Table: R", &header);
    table_print(&table);
    printf("Empty? %d", table_is_empty(&table));
    printf("Full? %d", table_is_full(&table));

    // randomize table
    table_randomize(&table);
    table_print(&table);
    printf("Empty? %d", table_is_empty(&table));
    printf("Full? %d", table_is_full(&table));

    // clear table
    table_clear(&table);
    table_print(&table);
    printf("Empty? %d", table_is_empty(&table));
    printf("Full? %d", table_is_full(&table));

    // insert some tuples
    table_insert(&table, &(tuple_t){23, 32});
    table_insert(&table, &(tuple_t){34, 44});
    table_print(&table);
    printf("Empty? %d", table_is_empty(&table));
    printf("Full? %d", table_is_full(&table));

    table_free(&table);
    return 0;
}