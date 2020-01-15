#include "harness.h"

int load_table(table_t *table, const rel_t *rel, uint64_t col){
    uint64_t i, *column;
    assert(rel && table && col >= 0);


    column = rel->data + (rel->rows * col);
    table_init(table, rel->rows, "Table", NULL);
    for(i = 0; i < rel->rows; i++)
        tuple_copy(&(tuple_t){column[i], i}, table->tuples + i);
    table->next = rel->rows;
    return 0;
}

// load full relation column into matrix column
int load_matrix_full(matrix_t *matrix, const rel_t *rel, tuple_t *info){
    uint64_t i, *column;
    assert(matrix && rel && info);

    fprintf(stdout, "> loading FULL %lu.%lu into matrix\n", info->key, info->payload); // @@@@
    column = rel->data + rel->rows * info->payload;
    for(i = 0; i < rel->rows; i++)
        matrix_set(matrix, &(tuple_t){column[i], i}, i, info->key);

    fprintf(stdout, "done\n"); // @@@@
    return 0;
}

// load specified rows only of a relation column from virtual memory into a matrix column
int load_matrix_col_from_rel(matrix_t *matrix, const rel_t *rel, tuple_t *info){
    uint64_t *col, i;
    tuple_t *tpl;
    assert(matrix && rel);
    fprintf(stdout, "> loading %lu.%lu for given row ids into matrix...", info->key, info->payload); // @@@@

    col = rel->data + (rel->rows * info->payload);
    for(i = 0; i < matrix->rows; i++){
        if(tuple_isempty(tpl = matrix_get(matrix, i, info->key)))
            break;
        matrix_set(matrix, &(tuple_t){col[tpl->payload], tpl->payload}, i, info->key);
    }

    fprintf(stdout, "done\n"); // @@@@
    return 0;
}

int load_all(matrix_t *matrix, table_t *table, tuple_t *existing, tuple_t *other, rel_t **rels){
    assert(matrix && table && existing && other && rels);

    // if left relation is not loaded, load into matrix
    if(matrix_isempty(matrix) || tuple_isempty(matrix_get(matrix, 0, existing->key)))
        load_matrix_full(matrix, rels[existing->key], existing);

    // else if another column was loaded
    else if(matrix->col_map[existing->key].key != existing->payload)
        load_matrix_col_from_rel(matrix, rels[existing->key], existing);

    // load & sort the non-empty relation one on the specified column,  if necessary
    if(matrix->col_map[existing->key].key != existing->payload || matrix->col_map[existing->key].payload == NOT_SORTED){
        fprintf(stdout, "> sorting %lu.%lu into matrix...", existing->key, existing->payload); // @@@@
        mx_radix(matrix, existing->key);
        fprintf(stdout, "done\n"); // @@@@

        // mark the actual column this matrix column refers to for each relation of this join
        tuple_set(&(matrix->col_map[existing->key]), existing->payload, SORTED);
    }

    // load & sort the right relation in a table
    fprintf(stdout, "> loading and sorting %lu.%lu into table...", other->key, other->payload); // @@@@
    load_table(table, rels[other->key], other->payload);
    table_sort_wrapper(table);
    fprintf(stdout, "done\n"); // @@@@
    tuple_set(&(matrix->col_map[other->key]), other->payload, SORTED);
    return 0;
}


int mx_join(matrix_t *matrix, uint64_t col1, const table_t *table, uint64_t col2){
    int compare;
    uint64_t i_a[2] = {0, 0}, i_b[2] = {0, 0}, i, j, count = 0, col1_size;
    matrix_t result;
    assert(matrix && table);

    fprintf(stdout, "> merging relations: (%lu >< %lu)...", col1, col2); // @@@@
    matrix_init(&result, matrix->cols);
    col1_size = matrix_col_size(matrix, col1);
    while(i_a[0] <  col1_size && i_b[0] < table->next){

        // set second pointer of each table to the very next set of keys
        for(i_a[1] = i_a[0] + 1; 
            i_a[1] < col1_size
            && tuple_compare(matrix_get(matrix, i_a[0], col1), matrix_get(matrix, i_a[1], col1)) == 0;
            i_a[1]++);
        
        for(i_b[1] = i_b[0] + 1;
            i_b[1] < table->next && tuple_compare(table->tuples + i_b[0], table->tuples + i_b[1]) == 0;
            i_b[1]++);

        // if the two keys match
        if((compare = tuple_compare(matrix_get(matrix, i_a[0], col1), table->tuples + i_b[0])) == 0){

            // insert the cartesian-product of the two sets in the result
            for(i = i_a[0]; i < i_a[1]; i++){
                for(j = i_b[0]; j < i_b[1]; j++){
                    matrix_setrow(&result, matrix->data[i], count);
                    matrix_set(&result, table->tuples + j, count++, col2);
                }
            }
        }

        // move pointers accordingly
        if(compare <= 0)
            i_a[0] = i_a[1];
        if(compare >= 0) 
            i_b[0] = i_b[1];     
    }

    // get rid of temporary helping matrix
    matrix_content_swap(matrix, &result);
    matrix_free(&result);

    fprintf(stdout, "done\n"); //@@@@
    return 0;
}

int join(const table_t *table_a, const table_t *table_b, list_t *list){
    int compare;
    uint64_t i_a[2] = {0, 0}, i_b[2] = {0, 0}, i, j;
    header_t header = {(field_t[]){{"Row_id-0", sizeof(uint64_t)}, {"Row_id-1", sizeof(uint64_t)}}, 2};
    assert(table_a && table_b && list);

    while(i_a[0] < table_a->next && i_b[0] < table_b->next){

        // set second pointer of each table to the very next set of keys
        for(i_a[1] = i_a[0] + 1; 
            i_a[1] < table_a->next && tuple_compare( table_a->tuples + i_a[0], table_a->tuples + i_a[1]) == 0;
            i_a[1]++);
        
        for(i_b[1] = i_b[0] + 1;
            i_b[1] < table_b->next && tuple_compare(table_b->tuples + i_b[0], table_b->tuples + i_b[1]) == 0;
            i_b[1]++);

        // if the two keys match
        if((compare = tuple_compare(table_a->tuples + i_a[0], table_b->tuples + i_b[0])) == 0){

            // insert the cartesian-product of the two sets in the result list
            for(i = i_a[0]; i < i_a[1]; i++){
                for(j = i_b[0]; j < i_b[1]; j++){
                    // create a new table-node if the list is empty or the very last one is full
                    if(list_is_empty(list) || table_is_full(list->tail->data))
                        list_insert(list, LIST_END, MAX_TABLE_SIZE / sizeof(tuple_t), "Result Table", &header);
                    table_insert(list->tail->data, &(tuple_t){table_a->tuples[i].payload, table_b->tuples[j].payload});
                }
            }
        }

        // move pointers accordingly
        if(compare <= 0)
            i_a[0] = i_a[1];
        if(compare >= 0) 
            i_b[0] = i_b[1];     
    }

    return 0;
}