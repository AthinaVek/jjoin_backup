#include "table.h"

int table_init_wrap(void *table, va_list props){
    int rv;
    if(table == NULL)
        return -1;

    // deconstruct arguments
    uint64_t size = va_arg(props, uint64_t);
    char *name = va_arg(props, char*);
    header_t *header = va_arg(props, header_t*);

    // initialize table appropriately
    rv = table_init(table, size, name, header);
    return rv;
}

int table_init(void *table, const uint64_t size, const char *name, const header_t *header){
    uint64_t i;
    table_t *this = table;
    if(table == NULL)
        return -1;

    // allocate memory for table name
    if(name != NULL){
        this->name = malloc(strlen(name) + 1);
        strcpy(this->name, name);
    }
    else
        this->name = NULL;

    // create table header
    if(header != NULL)
        header_init(&(this->header), header->fields, header->size);
    else
        this->header = (header_t){NULL, 0};

    // allocate memory for each row/tuple
    this->size = size;
    this->tuples = malloc(this->size * sizeof(tuple_t));

    // initialize each row/tuple
    for(i = 0; i < this->size; i++)
        tuple_init(this->tuples + i);

    // set first position as the next one available
    this->next = 0;

    return 0;
}

// insert a tuple to the next position available & increment table.next pointer
int table_insert(void *table, tuple_t *tuple){
    table_t *this = table;
    if(table == NULL || tuple == NULL)
        return -1;

    if(table_is_full(this))
        return -2;
    
    return tuple_copy(tuple, this->tuples + (this->next++));
}

int table_is_empty(const table_t *table){
    if(table == NULL)
        return -1;

    return table->next == 0;
}

int table_is_full(const void *table){
    const table_t *this = table;
    if(table == NULL)
        return -1;

    return this->next == this->size;
}

int table_clear(table_t *table){
    if(table == NULL)
        return -1;
    
    // set next available position to 0, so that all previous tuples get overriden
    table->next = 0;
    return 0;
}

// free all resources allocated for a table
int table_free(void *table){
    table_t *this = (table_t *)table;
    if(table == NULL)
        return -1;
        
    // free resources allocated for table name & data
    reset(this->name);
    reset(this->tuples);

    // free resources allocated for the header
    header_free(&(this->header));
    
    return 0;
}

int table_print(void *table){
    table_t *this = (table_t*)table;
    if(table == NULL)
        return -1;
    
    // print table name
    fprintf(stdout, "\n\e[1;4m%s\n\e[0m",(this->name != NULL ? this->name : "Unknown Table"));

    // print table header, columns seperated by 2 tabs
    header_print(&(this->header));

    // pretty print all table rows
    table_map(this, tuple_print);
    return 0;
}

int table_clone(const table_t *src, table_t *dst){
    if(src == NULL || dst == NULL || src->next > dst->size)
        return -1;
    
    return table_copy(src, &(tuple_t){0, src->next}, dst, 0);
}

// copy each tuple in range [from, to] of table src to the table dst, starting from the requested index
int table_copy(const table_t *src, const tuple_t *src_range, table_t *dst, uint64_t dst_from){
    uint64_t src_from, src_to;

    if(src == NULL || dst == NULL || src_range == NULL)
        return -1;

    // deconstruct ranges of src & make sure ranges are ok
    src_from = src_range->key; src_to = src_range->payload;
    if(src_from < 0 || src_to > src->size || dst_from < 0 || (src_to-src_from) > (dst->size-dst_from))
        return -2;

    // copy the appropriate tuples of src to the beginning of dst
    memcpy(dst->tuples + dst_from, src->tuples + src_from, sizeof(tuple_t) * (src_to - src_from));

    // update next available position of source 
    dst->next = MAX(dst->next, dst_from + (src_to - src_from)); 
    return 0;
}

int table_equals(const table_t *table_a, const table_t *table_b){
    uint64_t i;
    if(table_a == NULL || table_b == NULL || table_a->next != table_b->next)
        return -1;

    for(i = 0; i < table_a->next; i++){
        if(tuple_compare(table_a->tuples + i, table_b->tuples + i) != 0)
            return 0;
    }
    return 1;
}

int table_map(table_t *table, int (*tuple_function)(void *)){
    uint64_t i;
    if(table == NULL)
        return -1;

    // apply a function(e.g print or free) to each table row
    for(i = 0; i < table->next; i++){
        if(tuple_function(table->tuples + i) < 0)
            return -1;
    }

    return 0;
}