#include <stdio.h>
#include <time.h>
#include "matrix.h"
#define MAX_NUM 1000
#define COLUMNS 5

int main(){
    matrix_t matrix;

    srand(time(NULL));
    matrix_init(&matrix, COLUMNS);
    
    // insert item at a specified position
    for(uint64_t i = 0; i < COLUMNS; i++){
        matrix_set(&matrix, &(tuple_t){rand() % MAX_NUM + 1, i}, i, i);
        matrix_set(&matrix, &(tuple_t){rand() % MAX_NUM + 1, i}, COLUMNS-1-i, i);
    }

    matrix_print(&matrix);
    matrix_finalize(&matrix); // shrink to fit content
    matrix_print(&matrix);
    fprintf(stdout, "Last value: (%ld, %ld)\n", 
    matrix.data[matrix.rows-1][matrix.cols-1].key, matrix.data[matrix.rows-1][matrix.cols-1].payload);

    matrix_free(&matrix);
}