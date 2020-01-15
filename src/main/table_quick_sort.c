#include "table.h"

uint64_t table_partition(table_t *table, const tuple_t *range){
    uint64_t start, end, i, j;
    tuple_t *pivot;

    // demand of accurate arguments
    assert(table && range);

    // deconstruct arguments
    start = range->key; end = range->payload;

    // set pivot to be the last element
    pivot = &(table->tuples[end-1]);

    // i is the crossing line between smaller and larger(or not-yet-checked) elements than pivot
    for(i = start, j = i; j < end - 1; j++){
        if(tuple_compare(&(table->tuples[j]), pivot) < 0)
            tuple_swap(&(table->tuples[i++]), &(table->tuples[j]));
    }
    tuple_swap(&(table->tuples[i]), pivot);

    return i;
}

int table_quick_sort(table_t *table, const tuple_t *range){
    uint64_t start, end;
    assert(table && range);

    // deconstruct arguments
    start = range->key; end = range->payload;
    if(start < 0 || end < 1) 
        return -2;

    if(start < end-1){
        uint64_t pi = table_partition(table, &(tuple_t){start, end});
        table_quick_sort(table, &(tuple_t){start, pi});
        table_quick_sort(table, &(tuple_t){pi + 1, end});
    }

    return 0;
}
