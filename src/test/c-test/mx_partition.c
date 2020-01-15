#include <define.h>
#include "matrix.h"
#define COLS 3
#define ROWS 10
#define MAX_NUM 100

int main(int argc, char *argv[]){

    int64_t pivot;
    matrix_t matrix; 
    uint64_t size = ROWS, i, start, end, col, j;

    // get matrix size from cmd (optional)
    for(i = 1; i < argc; i++)
        if(strcmp(argv[i], "-s") == 0 && i < argc - 1)
            size = atoi(argv[i + 1]);

    // initialize matrix
    matrix_init(&matrix, COLS);
    matrix_randomize(&matrix, size, MAX_NUM);
    matrix_finalize(&matrix);
    matrix_print(&matrix);

    // partition matrix & display result
    fprintf(stdout, "\n\n\e[1;4mPartition\n\e[0m");
    start = matrix.rows/4;
    end = 3 * (matrix.rows/4);
    col = rand() % COLS;
    pivot = matrix_partition(&matrix, col, &(tuple_t){start, end});
    for(i = 0; i < matrix.rows; i++){
        for(j = 0; j < matrix.cols; j++){
            fputc('(', stdout);
            if(j == col && (i >= start && i < end))
                fprintf(stdout, "\e[1;32m");
            if(j == col && i == pivot)
                fprintf(stdout, "\e[7m");
            
            fprintf(stdout, "%4ld\e[0m, %4ld)\t\e[0m", matrix.data[i][j].key, matrix.data[i][j].payload);
        }
        fputc('\n', stdout);
    }

    // test result
    if(pivot < start || pivot > end){
        fprintf(stdout, "\e[1;31mError\e[0m: Pivot %ld is not within [%lu, %lu)\n", pivot, start, end);
        matrix_free(&matrix);
        return 1;
    }

    for(i = start; i < end; i++){

        // smaller elements must come before the chosen one
        if(i < pivot && matrix.data[i][col].key >= matrix.data[pivot][col].key){
            fprintf(stdout, "\e[1;31mError\e[0m at index %lu -> %lu >= %lu)\n", i, matrix.data[i][col].key, matrix.data[pivot][col].key);
            break;
        }
        // greater than or equal must come right after
        else if(i > pivot && matrix.data[i][col].key < matrix.data[pivot][col].key){
            fprintf(stdout, "\e[1;31mError\e[0m at index %lu -> %lu < %lu)\n", i, matrix.data[i][col].key, matrix.data[pivot][col].key);
            break;
        }
    }
    if(i == end)
        fprintf(stdout, "\e[1;32mSuccess\e[0m\n");

    matrix_free(&matrix);
    return 0;
}