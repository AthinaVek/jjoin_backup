#ifndef query_H_
#define query_H_

    #include "define.h"
    #include "list.h"
    #include "table.h"
    #include "tuple.h"
    #include "matrix.h"

    #define MAX_QUERY 100
    #define REL_DELIM " "
    #define PRED_DELIM "&"
    #define PROJ_DELIM " "
    #define FILTERS 3

    typedef struct pred{
        tuple_t left;
        tuple_t right;
        char operation;
    }pred_t;

    typedef struct query{
        rel_t **rels;
        uint64_t total;
        list_t preds;
        list_t projs;
    }query_t;

    // projection member methods
    int proj_init(void *, va_list);
    int proj_print(void*);

    // predicate member methods
    int pred_init(void *, va_list);
    int pred_print(void*);
    int pred_cmp(const void *, const void *);

    // query member methods
    int query_init(void *, va_list);
    int query_print(void *);
    int query_destroy(void *);

    // utility methods
    int is_join(const char*);
    int is_pred_join(const pred_t*);

    // operations
    int query_exec(void *);
    int query_filter(query_t *, pred_t *, matrix_t*);
    int query_filter_all(rel_t *, pred_t *, matrix_t*);
    int query_filter_existing(query_t *, pred_t *, matrix_t*);
    int query_join_existing(query_t *, pred_t *, matrix_t*);
    int filter_pass(uint64_t, uint64_t, char);
    int query_join(query_t *, pred_t *, matrix_t*);
    uint64_t query_projection(query_t *, tuple_t *, matrix_t*, uint8_t);
    uint64_t pred_value(pred_t *, rel_t *, uint64_t);

#endif