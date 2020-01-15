#ifndef harness_H
#define harness_H
    #include "table.h"

    #define MAX_PATH 512
    #define PATH_TERM "done"
    #define BATCH_TERM "F"
    #define QUERY_DELIM "|"

    // batch management methods
    int batch_list_init(void *, va_list);
    int batch_list_populate(void *, void *);
    int batch_exec(void *);

    // relation methods
    int rel_list_init(list_t *);

    // join operation methods
    int load_table(table_t *, const rel_t *, uint64_t);
    int load_matrix_full(matrix_t *, const rel_t *, tuple_t *);
    int load_matrix_col_from_rel(matrix_t *, const rel_t *, tuple_t*);
    int load_all(matrix_t *, table_t *, tuple_t *, tuple_t *, rel_t **);
    int join(const table_t *, const table_t *, list_t*);
    int mx_join(matrix_t *, uint64_t, const table_t *, uint64_t);

#endif