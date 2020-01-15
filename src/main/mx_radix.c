#include "table.h"

int mx_radix(matrix_t *matrix, uint64_t col){
    table_t table;
    assert(matrix);

    // pass a temporary table to use as storage
    table_init(&table, matrix_col_size(matrix, col), NULL, NULL);
    table.next = table.size; //@@@@
    matrix_finalize(matrix); // get rid of empty rows
    matrix_sort(matrix, &table, col,
               &(tuple_t){FIRST_CHUNK, CHUNK_SIZE}, &(tuple_t){0, matrix_col_size(matrix, col)});
    table_free(&table);
    return 0;
}

int matrix_sort(matrix_t *matrix, table_t *table, uint64_t col, 
                const tuple_t *chunk, tuple_t *range){
    table_t prefix_map;
    uint64_t i, offset, prefix, start, end, chunk_index, chunk_size, first_occurrence;
    assert(matrix && table && chunk && range);

    // deconstruct arguments
    start = range->key; end = range->payload;
    chunk_index = chunk->key; chunk_size = chunk->payload;

    // create histogram of our matrix column or table
    table_init(&prefix_map, (1 << chunk_size), NULL, NULL);
    if(chunk_index % 2)
        set_histogram(matrix, &prefix_map, chunk, range, col);
    else
        set_histogram(table, &prefix_map, chunk, range, UINT64_MAX);

    // create prefix sum out of histogram based on the starting index, keep track of first occurrence
    first_occurrence = prefix_map.size;
    set_prefix_sum(&prefix_map, &first_occurrence);

    // place each tuple in the right position of the re-ordered matrix column or table & update prefix sum
    for(i = start; i < end ; i++){
        prefix = get_chunk(chunk_index % 2 ? matrix->data[i][col].key : table->tuples[i].key, chunk_size, chunk_index);
        offset = start + prefix_map.tuples[prefix].payload++;
        if(chunk_index % 2 == 0)
            tuple_copy(table->tuples + i, matrix->data[offset] + col);
        else
            tuple_copy(matrix->data[i] + col, table->tuples + offset);
    }

    matrix_split(matrix, table, col, &prefix_map, chunk, range, first_occurrence);
    table_free(&prefix_map);

    return 0;
}

int matrix_split(matrix_t *matrix, table_t *table, uint64_t col, const table_t *prefix_map,
               const tuple_t* chunk, const tuple_t *range, const uint64_t first_occurrence){
    uint64_t i, limit, start, chunk_index, chunk_size, end;
    assert(matrix && table && prefix_map && chunk);


    // deconstruct arguments
    chunk_index = chunk->key; chunk_size = chunk->payload;
    start = range->key; end = range->payload;

    // for each prefix, if tuples of that prefix are too many, keep splitting or else sort them
    if((i = first_occurrence) < prefix_map->size){

        // using the modified version of prefix-sum, break matrix into chunks of same prefix for a given bit-set
        for(limit = start; limit < end ; start = limit){

            // get end-index of a chunk based on the offset of the prefix and the starting index
            limit = (i == prefix_map->size) ? end : range->key + prefix_map->tuples[i++].payload;

            // if bucket is small enough, quick-sort it
            if(sizeof(tuple_t) * (limit - start) < MAX_BUCKET_SIZE){
                
                if(chunk_index % 2)
                    table_to_mx(table, matrix, col, &(tuple_t){start, limit}, start);
                matrix_qsort(matrix, col, &(tuple_t){start, limit});
            }

            // if size of bucket is too large, keep on splitting it recursively
            if(chunk_index > 0){
                // sort by the very next set of bits, re-using a chunk of space from the initial matrix
                matrix_sort(matrix, table, col, &(tuple_t){chunk_index-1, chunk_size},
                           &((tuple_t){start, limit}));
                if(chunk_index % 2)
                    mx_to_table(matrix, table, col, &(tuple_t){start, limit}, start);
                else
                    table_to_mx(table, matrix, col, &(tuple_t){start, limit}, start);
            }

            // get index of the next prefix with at least one occurrence
            while(i < prefix_map->size){
                if(prefix_map->tuples[i].payload > 0)
                    break;
                i++;
            }
        }
    }

    return 0;
}