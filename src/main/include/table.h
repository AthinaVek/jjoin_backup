#ifndef table_H_
#define table_H_

    #include "define.h"
    #include "matrix.h"
    #include "utils.h"
    #include "header.h"
    #include "vector.h"
    #include "list.h"

    // data structures definitions
    typedef struct table{

        // data
        header_t header;
        tuple_t *tuples;

        // meta-data
        uint64_t next;
        uint64_t size;
        char *name;
    }table_t;

    // member methods
    int table_init_wrap(void *, va_list);
    int table_init(void *, const uint64_t, const char *, const header_t *);
    int table_insert(void *, tuple_t *);
    int table_is_empty(const table_t *);
    int table_is_full(const void *);
    int table_clear(table_t *);
    int table_print(void *);
    int table_free(void *);
    int table_equals(const table_t *, const table_t *);
    int table_clone(const table_t*, table_t *);
    int table_copy(const table_t*, const tuple_t *, table_t*, uint64_t);
    int table_map(table_t *, int (*)(void *));

    // helper methods
    int table_randomize(table_t *); 
    int table_by_row(table_t *);
    int table_reset(table_t *, const char *, const header_t *);
    int table_reducer(const void*, void*, ...);
    int table_to_mx(table_t*, matrix_t *, uint64_t, tuple_t*, uint64_t);
    int mx_to_table(matrix_t*, table_t *, uint64_t, tuple_t*, uint64_t);

    // sort-merge-join method declarations
    int table_sort_wrapper(table_t *);
    int table_sort(table_t*, table_t *, const tuple_t*, tuple_t*); // radix
    int table_split(table_t *, table_t *, const table_t *, const tuple_t*, const tuple_t *, const uint64_t); // radix
    int table_quick_sort(table_t *, const tuple_t *);
    uint64_t table_partition(table_t *, const tuple_t *);

    int set_histogram(const void *, table_t *, const tuple_t*, const tuple_t*, uint64_t);
    int set_prefix_sum(table_t *, uint64_t *);

    // matrix-integration methods
    int mx_radix(matrix_t *, uint64_t);
    int matrix_sort(matrix_t *, table_t *, uint64_t, const tuple_t *, tuple_t *);
    int matrix_split(matrix_t *, table_t *, uint64_t, const table_t *, 
                     const tuple_t*, const tuple_t *, const uint64_t);
                     
#endif