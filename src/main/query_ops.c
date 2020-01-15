#include "query.h"
#include "harness.h"

int query_exec(void *query){
    query_t *q = query;
    node_t *i;
    matrix_t matrix;
    uint8_t empty = 0;

    assert(query);
    query_print(query);
    matrix_init(&matrix, q->total);

    // execute filters
    for(i = q->preds.head; i != NULL && !is_pred_join(i->data) && !empty; i = i->next)
        empty = query_filter(q, i->data, &matrix);
    if(empty) goto done;

    // re-order the remaining predicates, they are all joins
    if(i != NULL && i->prev != NULL)
        i = i->prev->next = nodelist_mergesort(i, pred_cmp);
    else if(i == q->preds.head)
        i = q->preds.head = nodelist_mergesort(i, pred_cmp);

    // execute joins
    while(i != NULL && !empty){
        empty = query_join(q, i->data, &matrix);
        i = i->next;
    }
    if(empty) goto done;

    // remove empty rows
    matrix_finalize(&matrix);

done:
    // display columns sums requested
    for(node_t *i = q->projs.head; i != NULL; i = i->next)    
        query_projection(query, i->data, &matrix, i->next == NULL);

    matrix_free(&matrix);
    return 0;
}

int query_filter(query_t *query, pred_t *pred, matrix_t *matrix){
    assert(query && pred && matrix);

    // display log message
    fprintf(stdout, "\n\e[1;4mExecuting filter...\n");
    pred_print(pred);

    // if this relationship has not yet been filtered ever, filter it all, or else just the remaining part
    if(matrix_isempty(matrix) || matrix_empty_row(matrix, 0))
        return query_filter_all(query->rels[pred->left.key], pred, matrix);
    else
        return query_filter_existing(query, pred, matrix);
}
    
int query_filter_all(rel_t *rel, pred_t *pred, matrix_t *matrix){
    uint64_t row_id, value, i, *col;
    assert(rel && pred && matrix);

    // check all rows against the filter and keep those that pass
    col = rel->data + (pred->left.payload * rel->rows);
    for(i = row_id = 0; row_id < rel->rows; row_id++){
        value = pred_value(pred, rel, row_id);

        // add row id to result if it passes the filter
        if(filter_pass(col[row_id], value, pred->operation))
            matrix_set(matrix, &(tuple_t){col[row_id], row_id}, i++, pred->left.key);
    }

    return matrix_isempty(matrix);
}

int query_filter_existing(query_t *query, pred_t *pred, matrix_t *matrix){
    matrix_t tmp;
    rel_t *rel;
    uint64_t rel_index, column_index, i, j, count, *col;
    assert(query && pred && matrix);
    matrix_init(&tmp, matrix->cols);

    rel_index = pred->left.key;
    column_index = pred->left.payload;
    rel = query->rels[pred->left.key];
    col = rel->data + (column_index * rel->rows);
    matrix_print(matrix);

    // iterate on the column to be filtered of the new relation
    for(i = count = 0; i < rel->rows; i++){
        
        // add cartesian-product with the intermediate result in the matrix, if this is a match
        if(filter_pass(col[i], pred->right.key, pred->operation)){
            for(j = 0; j < matrix->rows && !matrix_empty_row(matrix, j); j++){
                matrix_setrow(&tmp, matrix->data[j], count);
                matrix_set(&tmp, &(tuple_t){col[i], i}, count++, rel_index);
            }
        }
    }

    // get rid of temporary helping matrix
    matrix_content_swap(matrix, &tmp);
    matrix_free(&tmp);

    return 0;
}

int query_join_existing(query_t *query, pred_t *pred, matrix_t *matrix){
    uint64_t left_id, value, i, j, *col, right_id;
    rel_t *left_rel, *right_rel;
    assert(query && pred && matrix);

    // relationship data for the left part of the filter
    left_rel = query->rels[pred->left.key];

    // get data for the column we are about to filter
    col = left_rel->data + (pred->left.payload * left_rel->rows);

    // from existing row ids, keep  those that pass against the filter
    i = j = 0; // i points to the next position available, j is looking for the next appropriate row id

    fprintf(stdout, "> both relations exist, applying equality filter: %lu.%lu = %lu.%lu...",
                    pred->left.key, pred->left.payload, pred->right.key, pred->right.payload); // @@@@
    while(j < matrix->rows && !matrix_empty_row(matrix, j)){
        left_id = matrix_get(matrix, j, pred->left.key)->payload;
        right_rel = query->rels[pred->right.key];
        right_id = matrix_get(matrix, j, pred->right.key)->payload;
        value = (right_rel->data + (pred->right.payload * right_rel->rows))[right_id];

        // move row up if it passes the filter
        if(filter_pass(col[left_id], value, pred->operation) && i <= j)
            memmove(matrix->data[i++], matrix->data[j], sizeof(tuple_t) * matrix->cols);
        j++; // next row
    }

    // mark the rest of the rows as empty
    while(i < matrix->rows){
        for(j = 0; j < matrix->cols; j++)
            matrix_set(matrix, &(tuple_t){UINT64_MAX, UINT64_MAX}, i, j);
        i++;
    }

    fprintf(stdout, "done\n"); // @@@@
    return matrix_isempty(matrix);
}

int filter_pass(uint64_t left_side, uint64_t right_side, char operation){
    if(operation == '<')
        return left_side < right_side;
    else if(operation == '>')
        return left_side > right_side;
    else
        return left_side == right_side;
}

uint64_t query_projection(query_t *query, tuple_t *proj, matrix_t *matrix, uint8_t last){
    uint64_t sum, i, *col;
    rel_t *rel;
    FILE *log;
    assert(query && matrix && proj);

    if(proj->key >= matrix->cols)
        return 0;

    // get column to sum up
    rel = query->rels[proj->key];
    col = rel->data + (proj->payload * rel->rows);

    // for each row_id add column value to the result
    for(sum = i = 0; i < matrix->rows; i++){
        if(tuple_isempty(matrix->data[i] + proj->key))
            break;
        sum += col[matrix->data[i][proj->key].payload];
    }

    fprintf(stdout, "Sum of \e[4m%lu.%lu\e[0m: %lu\n", proj->key, proj->payload, sum);

    // write sum into a log file
    log = fopen(LOG, "a");
    if(sum > 0)
        fprintf(log, "%lu", sum);
    else 
        fprintf(log, "NULL");
    fputc(last ? '\n' : ' ', log);
    fclose(log);

    return sum;
}

int query_join(query_t *query, pred_t *pred, matrix_t *matrix){
    table_t tbl;
    int left_loaded, right_loaded;
    assert(query && pred && matrix);

    // display log message
    fprintf(stdout, "\n\e[1;4mExecuting join...\n");
    pred_print(pred);

    // determine whether any of the relations is already loaded in the matrix
    left_loaded = !matrix_isempty(matrix) && !(tuple_isempty(matrix_get(matrix, 0, pred->left.key)));
    right_loaded = !matrix_isempty(matrix) && !(tuple_isempty(matrix_get(matrix, 0, pred->right.key)));

    // if both relations are loaded filter existing rows
    if(left_loaded && right_loaded)
        return query_join_existing(query, pred, matrix);

    // else if right relation is already loaded, load left into the table
    else if(right_loaded)
        load_all(matrix, &tbl, &(pred->right), &(pred->left), query->rels);

    // else if load left in the matrix(if it is not loaded already) and the right in the table
    else
        load_all(matrix, &tbl, &(pred->left), &(pred->right), query->rels);

    fprintf(stdout, "> all relations loaded\n");

    // join a matrix column with the table
    mx_join(matrix, right_loaded ? pred->right.key : pred->left.key,
            &tbl, right_loaded ? pred->left.key : pred->right.key);

    table_free(&tbl);
    return matrix_isempty(matrix);
}

uint64_t pred_value(pred_t *pred, rel_t *rel, uint64_t row_id){
    assert(pred && rel);

    // get value for the right part of the filter
    if(pred->right.payload == UINT64_MAX) // a constant
        return pred->right.key; 
    else if(pred->left.key == pred->right.key)// a column value of this relationship
        return (rel->data + (pred->right.payload * rel->rows))[row_id];
    else    
        return UINT64_MAX;
}