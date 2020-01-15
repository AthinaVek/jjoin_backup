#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "matrix.h"
#include "utils.h"

int matrix_init(void *matrix, uint64_t cols){
    matrix_t *m = matrix;
    assert(matrix);

    m->data = NULL;
    m->rows = 0;
    m->cols = cols;

    // map each column to the # of actual column it refers to
    m->col_map = malloc(cols * sizeof(tuple_t));
    memset(m->col_map, UINT32_MAX, cols * sizeof(tuple_t)); // fill with 1s
    return 0;
}

// add rows to a matrix, filled with -1s indicating empty spots
void matrix_resize(void *matrix, int64_t rows){
    uint64_t i;
    matrix_t *m = matrix;
    assert(matrix);

    // if this is a shrinking
    if(rows < 0){
        for(uint64_t i = m->rows + rows; i < m->rows; i++)
            reset(m->data[i]);
    }

    // update matrix
    m->rows += rows;
    m->data = realloc(m->data, m->rows * sizeof(tuple_t*));

    // fill added rows with -1s
    for(i = m->rows - rows; i < m->rows; i++){
        m->data[i] = malloc(m->cols * sizeof(tuple_t));
        memset(m->data[i], UINT32_MAX, m->cols * sizeof(tuple_t));
    }
}

void matrix_create_space(void *matrix, uint64_t row){
    matrix_t *m = matrix;
    uint64_t capacity_required;
    assert(matrix);

    capacity_required = nearest_power_of2(row + 1, BITS);
    if(m->rows == 0) // if matrix is empty
        matrix_resize(matrix, MATRIX_INITIAL_CAPACITY);
    else if(capacity_required > m->rows) // double the # of rows, if there aren't enough
        matrix_resize(matrix, capacity_required - m->rows);
}

int matrix_set(void *matrix, tuple_t *value, uint64_t row, uint64_t col){
    matrix_t *m = matrix;
    assert(matrix);
    if(col > m->cols-1)
        return -1;

    matrix_create_space(matrix, row);
    return tuple_copy(value, &(m->data[row][col]));
}

int matrix_setrow(void *matrix, tuple_t *row, uint64_t index){
    matrix_t *m = matrix;
    assert(matrix);

    matrix_create_space(matrix, index);
    memcpy(m->data[index], row, sizeof(tuple_t) * m->cols);
    return 0;
}

tuple_t * matrix_get(void *matrix, uint64_t i, uint64_t j){
    matrix_t *m = matrix;
    assert(matrix && i < m->rows && j < m->cols);
    return m->data[i] + j;
}

int matrix_empty_row(void *matrix, uint64_t row){
    int i;
    matrix_t *m = matrix;
    assert(matrix);

    if(row >= m->rows)
        return -1;
    
    for(i = 0; i < m->cols && tuple_isempty(m->data[row] + i); i++);
    return i == m->cols;
}

uint64_t matrix_finalize(void *matrix){
    matrix_t *m = matrix;
    uint64_t i;
    assert(matrix);

    // release consecutive bottom-most empty rows
    for(i = m->rows-1; i >= 0 && matrix_empty_row(matrix, i) > 0; i--);
    if(i < m->rows - 1)
        matrix_resize(matrix, i + 1 - m->rows);

    return m->rows - (i + 1); // the number of rows released
}

void matrix_print(void *matrix){
    uint64_t i;
    matrix_t *m = matrix;
    assert(matrix);

    // display meta-data
    fprintf(stdout, 
            "\e[1;4mMatrix\e[0m\n * rows: %lu\n * cols: %lu\n * col_map: [ ", 
            m->rows, m->cols);
    for(i = 0; i < m->cols; i++)
        fprintf(stdout, " (%ld, %ld) ", m->col_map[i].key, m->col_map[i].payload);

    // display actual data
    fprintf(stdout, "]\n * content\n");
    for(i = 0; i < m->rows && !matrix_empty_row(matrix, i); i++)
        matrix_rowprint(matrix, i);

    if(i < m->rows)
        fprintf(stdout, " \e[1;4m%lu\e[0m empty row(s) follow...\n", m->rows - i);
}

void matrix_rowprint(void *matrix, uint64_t row){
    matrix_t *m = matrix;
    assert(matrix && row < m->rows);

    for(uint64_t j = 0; j < m->cols; j++)
        fprintf(stdout, "(%-6ld, %6ld)\t", m->data[row][j].key, m->data[row][j].payload);
    fputc('\n', stdout);
}

void matrix_free(void *matrix){
    uint64_t i;
    matrix_t *m = matrix;
    assert(matrix);

    for(i = 0; i < m->rows; i++)
        reset(m->data[i]);
    reset(m->data);
    reset(m->col_map);
}

int matrix_isempty(void *matrix){
    matrix_t *m = matrix;
    assert(matrix);
    return m->rows == 0;
}

uint64_t matrix_col_size(void *matrix, uint64_t col){
    uint64_t i;
    matrix_t *m = matrix;
    assert(matrix);

    for(i = 0; i < m->rows && !tuple_isempty(matrix_get(matrix, i, col)); i++);
    return i;
}