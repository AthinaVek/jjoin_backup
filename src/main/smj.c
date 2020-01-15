#include "table.h"

int set_histogram(const void *src, table_t *histogram, 
                 const tuple_t *chunk, const tuple_t *range, uint64_t col){
    uint64_t i, prefix, start, end, chunk_index, chunk_size; 
    const char table_name[] = "Histogram";
    header_t histogram_header = {(field_t[]){{"Key", sizeof(uint64_t)}, {"Occurrences", sizeof(uint64_t)}}, 2};

    assert(src && histogram && chunk && range);

    // deconstruct arguments
    start = range->key; end = range->payload;
    chunk_index = chunk->key; chunk_size = chunk->payload;

    // table must have at least 2^chunk_size rows to satisfy all possible values
    assert(histogram->size >= (1 << chunk_size));

    // rename table & fields to 'Histogram' & 'Key', 'Occurrences'
    string_rename(&(histogram->name), table_name);
    header_reset(&(histogram->header), &histogram_header);

    // set key equal to index for each tuple
    for(i = 0; i < histogram->size; i++)
        histogram->tuples[i].key = i;

    for(i = start; i < end; i++){

        // set prefix as the $chunk_index-th set of $chunk_size BITS
        if(chunk_index * chunk_size > BITS)
            return -3;
        prefix = get_chunk(col == UINT64_MAX ? ((table_t*)src)->tuples[i].key : ((matrix_t*)src)->data[i][col].key,
                 chunk_size, chunk_index);

        if(histogram->size < prefix + 1)
            return -4;
        
        histogram->tuples[prefix].payload++; 
    }

    histogram->next = histogram->size;
    return 0;
}

int set_prefix_sum(table_t *histogram, uint64_t *rv){
    const char name[] = "Prefix Sum";
    uint8_t flag;
    uint64_t i, sum;
    header_t prefix_sum_header = {(field_t[]){{"Key", sizeof(uint64_t)}, {"Offset", sizeof(uint64_t)}}, 2};

    assert(histogram && histogram->size > 0);

    // rename table name & fields to 'Prefix Sum' and 'Key', 'Offset'
    table_reset(histogram, name, &prefix_sum_header);
    for(flag = sum = i = 0; i < histogram->size; i++){
        sum += histogram->tuples[i].payload;
        if(histogram->tuples[i].payload > 0){
            histogram->tuples[i].payload = sum - histogram->tuples[i].payload;

            // keep track of the very first number that occurred
            if(!flag){
                flag = 1;
                *rv = histogram->tuples[i].key;
            }
        }
    }

    return 0;
}