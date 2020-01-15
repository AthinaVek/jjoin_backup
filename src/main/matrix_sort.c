#include "matrix.h"

uint64_t matrix_partition(matrix_t *matrix, uint64_t column, const tuple_t *range){
    uint64_t start, end, i, j;
    tuple_t *pivot;

    // demand of accurate arguments
    assert(matrix && range);

    // deconstruct arguments
    start = range->key; end = range->payload;

    // set pivot to be the last element
    pivot = &(matrix->data[end-1][column]);

    // i is the crossing line between smaller and larger(or not-yet-checked) elements than pivot
    for(i = start, j = i; j < end - 1; j++){
        if(tuple_compare(matrix->data[j] + column, pivot) < 0)
            tuple_swap(matrix->data[i++] + column, matrix->data[j] + column);
    }
    tuple_swap(matrix->data[i] + column, pivot);

    return i;
}

int matrix_qsort(matrix_t *matrix, uint64_t col, const tuple_t *range){
    uint64_t start, end;
    assert(matrix && range);

    // deconstruct arguments
    start = range->key; end = range->payload;
    if(start < 0 || end < 1) 
        return -2;

    if(start < end-1){
        uint64_t pi = matrix_partition(matrix, col, &(tuple_t){start, end});
        matrix_qsort(matrix, col, &(tuple_t){start, pi});
        matrix_qsort(matrix, col, &(tuple_t){pi + 1, end});
    }

    return 0;
}