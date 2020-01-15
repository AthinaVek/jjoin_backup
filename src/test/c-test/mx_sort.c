#include <define.h>
#include "table.h"

#define ROWS 10
#define COLS 3
#define SORT_COL 0
#define METHOD 0
#define MAX_NUM 10000
#define FLAGS 4

int main(int argc, char *argv[]){

    matrix_t matrix; 
    char *flags[FLAGS] = {"--rows", "--method", "--col", "--max"};
    uint64_t i, j, rows = ROWS, cols = COLS, column = SORT_COL, max_num = MAX_NUM, method = METHOD;
    uint64_t *args[FLAGS] = {&rows, &method, &column, &max_num};

    // get matrix size, sorting method and column index to sort from cmd (optional)
    for(i = 1; i < argc; i++){
        for(j = 0; j < FLAGS; j++)
            if(strcmp(flags[j], argv[i]) == 0){
                *(args[j]) = atoi(argv[i + 1]);
                break;
            }
    }
    if(column >= cols)
        return 1;

    // initialize, randomize, set row_id & display input matrix
    matrix_init(&matrix, cols);
    matrix_randomize(&matrix, rows, max_num);
    matrix_finalize(&matrix);
    matrix_print(&matrix);

    // sort the un-ordered matrix using the requested method, result goes into the ordered one
    if(method) 
        // quick-sort a given column in place!
        matrix_qsort(&matrix, column, &(tuple_t){0, matrix.rows}); // quick-sort

    else
        // radix-sort a given column in place!
        mx_radix(&matrix, column); // radix-sort
    
    // display sorted matrix
    fprintf(stdout, "\n\e[1;96m >> \e[4mAfter sorting on key of column %lu\e[0;1;96m <<\e[0m\n\n", column);
    matrix_print(&matrix);

    // ensure that the matrix is sorted ASC
    if((i = matrix_test_order(&matrix, column)) == 0)
        fprintf(stdout, "\n\e[1;32mSuccess\e[0m: sort method %lu succeeded\n", method);
    else
        fprintf(stdout, "\n\e[1;31mFailure\e[0m: sort method %lu failed on index %lu\n", method, i);

    // free all resources
    matrix_free(&matrix);
    return 0;
}