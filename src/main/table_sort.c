#include "table.h"

int table_sort_wrapper(table_t *table){
    table_t tmp;
    assert(table);

    table_init(&tmp, table->next, NULL, NULL);
    table_clone((const table_t*)table, &tmp);
    table_sort(&tmp, table, &(tuple_t){FIRST_CHUNK, CHUNK_SIZE}, &(tuple_t){0, tmp.next});
    table_free(&tmp);
    return 0;
}

int table_sort(table_t *table, table_t *table_r, const tuple_t *chunk, tuple_t *range){
    table_t prefix_map;
    uint64_t i, r_index, prefix, start, end, chunk_index, chunk_size, first_occurrence;
    assert(table && table->size > 0 && table->next <= table->size);

    // deconstruct arguments
    start = range->key; end = range->payload;
    chunk_index = chunk->key; chunk_size = chunk->payload;

    // re ordered table will eventually contain the same number of elements
    // with the to be sorted part of initial table
    if(end - start > table_r->size)
        return -2;

    // create histogram of our table
    table_init(&prefix_map, (1 << chunk_size), NULL, NULL);
    set_histogram(table, &prefix_map, chunk, range, UINT64_MAX);

    // create prefix sum out of histogram based on the starting index, keep track of first occurrence
    first_occurrence = prefix_map.size;
    set_prefix_sum(&prefix_map, &first_occurrence);

    // place each tuple in the right position of the re-ordered table & update prefix sum
    for(i = start; i < end ; i++){
        prefix = get_chunk(table->tuples[i].key, chunk_size, chunk_index);
        r_index = start + prefix_map.tuples[prefix].payload++;
        tuple_copy(table->tuples + i, table_r->tuples + r_index);
    }

    table_split(table, table_r, &prefix_map, chunk, range, first_occurrence);
    table_free(&prefix_map);

    table_r->next = end - start;
    return 0;
}

int table_split(table_t *table, table_t *table_r, const table_t *prefix_map,
               const tuple_t* chunk, const tuple_t *range, const uint64_t first_occurrence){
    uint64_t i, limit, start, chunk_index, chunk_size, end;
    assert(table && table_r && prefix_map && chunk);

    // deconstruct arguments
    chunk_index = chunk->key; chunk_size = chunk->payload;
    start = range->key; end = range->payload;

    // for each prefix, if tuples of that prefix are too many, keep splitting or else sort them
    if((i = first_occurrence) < prefix_map->size){

        // using the modified version of prefix-sum, break table into chunks of same prefix for a given bit-set
        for(limit = start; limit < end ; start = limit){

            // get end-index of a chunk based on the offset of the prefix and the starting index
            limit = (i == prefix_map->size) ? end : range->key + prefix_map->tuples[i++].payload;

            // if bucket is small enough, quick-sort it
            if(sizeof(tuple_t) * (limit - start) < MAX_BUCKET_SIZE)
                table_quick_sort(table_r, &(tuple_t){start, limit});

            // if size of bucket is too large, keep on splitting it recursively
            else if(chunk_index > 0){
                // sort by the very next set of bits, re-using a chunk of space from the initial table
                table_sort(table_r, table, &(tuple_t){chunk_index-1, chunk_size}, &((tuple_t){start, limit}));
                table_copy(table, &(tuple_t){start, limit}, table_r, start);
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
