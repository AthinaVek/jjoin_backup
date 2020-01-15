#include "matrix.h"

// insert random key - value in each tuple of all columns in this matrix
int matrix_randomize(matrix_t *matrix, uint64_t rows, uint64_t max){
    static uint8_t i = 0;
    assert(matrix);

    // use current time as random number generator seed
    srand(time(NULL) << i++);

    for(int r = 0; r < rows; r++){
        for(int c = 0; c < matrix->cols; c++)
            matrix_set(matrix, &(tuple_t){rand_bits() % max, rand_bits() % max}, r, c);
    }

    return 0;
}

// test whether a certain column of the matrix is key-sorted
int matrix_test_order(matrix_t *matrix, uint64_t col){
    uint64_t prev, i;
    assert(matrix);

    for(i = prev = 0; i < matrix->rows; prev = matrix->data[i][col].key, i++)
        if(matrix->data[i][col].key < prev)
            return i; // return index of failure
    return 0;
}

void matrix_content_swap(matrix_t *m0, matrix_t *m1){

    assert(m0 && m1);
    
    memswap(&(m0->rows), &(m1->rows), sizeof(uint64_t));
    memswap(&(m0->cols), &(m1->cols), sizeof(uint64_t));
    memswap(&(m0->data), &(m1->data), sizeof(tuple_t*));
}