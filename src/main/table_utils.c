#include "table.h"

// insert random key - value in each tuple of this table
int table_randomize(table_t *table){
    static uint8_t i = 0;
    assert(table);

    // declare that the whole table will be filled
    table->next = table->size;

    // use current time as random number generator seed
    srand(time(NULL) << i++);
    table_map(table, tuple_randomize);

    return 0;
}

// for each tuple, set payload to row_id
int table_by_row(table_t *table){
    uint64_t i;
    assert(table);

    for(i = 0; i < table->next; i++)
        table->tuples[i].payload = i;
    return 0;
}

int table_reset(table_t *table, const char *name, const header_t *header){
    assert(table && name && header);

    return string_rename(&(table->name), name) + header_reset(&(table->header), header);
}

// reducer function for table_t, accumulating the sum of sizes for a set of tables
int table_reducer(const void *table, void *total, ...){
    const table_t *this = table;
    uint64_t *acc = total;
    assert(table && total);

    *acc = *acc + this->next;
    return 0;
}

int table_to_mx(table_t* table, matrix_t *matrix, uint64_t col, tuple_t* range, uint64_t start){
    uint64_t from, to, i;
    assert(table && matrix && range);

    // deconstruct arguments
    from = range->key;
    to = range->payload;

    for(i = from; i < to; i++)
        tuple_copy(table->tuples + i, matrix->data[i - from + start] + col);

    return 0;
}

int mx_to_table(matrix_t* matrix, table_t *table, uint64_t col, tuple_t* range, uint64_t start){
    uint64_t from, to, i;
    assert(table && matrix && range);

    // deconstruct arguments
    from = range->key;
    to = range->payload;

    for(i = from; i < to; i++)
        tuple_copy(matrix->data[i] + col, table->tuples + start + i - from);

    return 0;
}
