#include <define.h>
#include "harness.h"

#define ROWS 3
#define COLS 3
#define SORT_COL 0
#define RES_COL 2
#define METHOD 0
#define MAX_NUM 1000
#define FLAGS 3

int cmp(const void*, const void*);
void print(const void*, uint64_t);

int main(int argc, char *argv[]){

    matrix_t matrix; 
    table_t table;
    char *flags[FLAGS] = {"--rows", "--method", "--max"};
    uint64_t i, j, j1, rows = ROWS, max_num = MAX_NUM, method = METHOD;
    uint64_t *args[FLAGS] = {&rows, &method, &max_num};
    header_t header = {(field_t[]){{"Key", sizeof(uint64_t)}, {"oID", sizeof(uint64_t)}}, 2};

    // get matrix size, sorting method and column index to sort from cmd (optional)
    for(i = 1; i < argc; i++){
        for(j = 0; j < FLAGS; j++)
            if(strcmp(flags[j], argv[i]) == 0){
                *(args[j]) = atoi(argv[i + 1]);
                break;
            }
    }

    // initialize matrix, table & array
    matrix_init(&matrix, COLS);
    table_init(&table, 3 * rows, "Table", &header);

    //randomize, set row_id for both matrix and table
    srand(time(NULL));
    for(i = 0; i < rows; i++){
        matrix_set(&matrix, &(tuple_t){j = rand() % max_num, i}, i, 0);
        matrix_set(&matrix, &(tuple_t){j1 = rand() % max_num, i}, i, 1);
        table_insert(&table, &(tuple_t){rand() % 2 ? j : j1, rows + i});
        table_insert(&table, &(tuple_t){rand() % 2 ? j : j1, 2 * rows + i});
        table_insert(&table, &(tuple_t){rand() % 2 ? j : j1, 3 * rows + i});
    }
    matrix_finalize(&matrix);

    // sort the un-ordered matrix using the requested method, result goes into the ordered one
    matrix_qsort(&matrix, SORT_COL, &(tuple_t){0, matrix.rows}); // quick-sort
    table_quick_sort(&table, &(tuple_t){0, table.next});

    // display sorted matrix and table
    fprintf(stdout, "\n >> \e[1;4mAfter sorting on key of column %u\e[0;1;96m <<\e[0m\n\n", SORT_COL);
    matrix_print(&matrix);
    table_print(&table);

    // merge matrix and table
    fprintf(stdout, "\n >> \e[1;4mMerging column %u and table into column %u << \e[0m\n", SORT_COL, RES_COL);
    mx_join(&matrix, SORT_COL, &table, RES_COL);
    matrix_print(&matrix);

    // free all resources
    matrix_free(&matrix);
    table_free(&table);
    return 0;
}

int cmp(const void *a, const void *b){
    assert(a && b);
    return memcmp(a, b, sizeof(uint64_t));
}

void print(const void* array, uint64_t len){
    const uint64_t *a = array;
    assert(array);

    fputc('[', stdout);
    for(uint64_t i = 0; i < len - 1; i++) 
        fprintf(stdout, "%lu\t", a[i]);
    fprintf(stdout, "%lu]\n", a[len-1]);
}