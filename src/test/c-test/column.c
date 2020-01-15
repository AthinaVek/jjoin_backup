#include "harness.h"
#define TESTS 2

int test_rel_sum(const rel_t *, uint64_t);
int test_rel_col_to_table(const rel_t *, uint64_t);
int main(int argc, char *argv[]){
    rel_t rel;
    uint64_t column, test;
    const int (*tests[TESTS])(const rel_t *, uint64_t) = {test_rel_sum, test_rel_col_to_table};

    // ensure that the relationship file, column & test numbers were all provided
    if(argc < 4)
        return error_return(1, "\e[1m_usage\e[0m: %s <rel-filepath> <column-index> <test-index>\n", argv[0]);

    if(rel_init(&rel, argv[1]) < 0)
        return 1;

    // ensure that indexes are within range
    if((column = atoi(argv[2])) < 0 || column >= rel.cols)
        return error_return(2, "\e[1;31mError\e[0m: Column index %lu out of bounds[0, %lu]\n", column, rel.cols-1);

    if((test = atoi(argv[3])) < 0 || test >= TESTS)
        return error_return(3, "\e[1;31mError\e[0m: Test index %lu out of bounds[0, %lu]\n", test, TESTS-1);

    // run the appropriate test
    return tests[atoi(argv[3])](&rel, column);
}

int test_rel_sum(const rel_t *rel, uint64_t col){
    uint64_t init = 0;
    if(rel == NULL || col < 0 || col >= rel->cols)
        return -1;

    rel_reduce(rel, &init, col, rel_add);
    fprintf(stdout, "\e[1mTotal\e[0m: %lu\n", init);
    return 0;
}

int test_rel_col_to_table(const rel_t *rel, uint64_t col){
    table_t tbl;
    if(rel == NULL || col < 0 || col >= rel->cols)
        return -1;
    table_init(&tbl, rel->rows, NULL, NULL);
    load_table(&tbl, rel, col);
    table_print(&tbl);
    table_free(&tbl);
    return 0;
}