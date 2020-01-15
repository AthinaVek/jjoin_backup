#ifndef matrix_H_
#define matrix_H_                           

    #include <stdlib.h>
    #include <stdint.h>
    #include <stdarg.h>
    #include "relation.h"
    #include "tuple.h"

    #define MATRIX_INITIAL_CAPACITY 4
    #define SORTED 1
    #define NOT_SORTED UINT64_MAX

    typedef struct matrix{
        tuple_t **data;
        tuple_t *col_map;
        uint64_t rows;
        uint64_t cols;
    }matrix_t;

    // member methods
    int matrix_init(void *, uint64_t);
    int matrix_set(void *, tuple_t*, uint64_t, uint64_t);
    int matrix_setrow(void *, tuple_t *, uint64_t);
    tuple_t * matrix_get(void *, uint64_t, uint64_t);
    uint64_t matrix_finalize(void *);
    void matrix_print(void*);
    void matrix_rowprint(void *, uint64_t);
    void matrix_free(void*);
    int matrix_isempty(void *);

    // utilities
    void matrix_create_space(void *, uint64_t);
    void matrix_resize(void*, int64_t);
    int matrix_randomize(matrix_t *, uint64_t, uint64_t);
    int matrix_empty_row(void *, uint64_t);
    void matrix_content_swap(matrix_t *, matrix_t *);

    // sorting-methods
    uint64_t matrix_col_size(void *, uint64_t);
    int matrix_qsort(matrix_t *, uint64_t, const tuple_t *);
    int matrix_test_order(matrix_t *, uint64_t);
    uint64_t matrix_partition(matrix_t *, uint64_t, const tuple_t *);

#endif