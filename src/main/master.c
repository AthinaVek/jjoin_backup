#include "master.h"

int vector_init(void *vector){
    vector64_t *v = vector;
    if(vector == NULL)
        return -1;

    v->next = 0;
    v->size = VECTOR_STARTING_SIZE;

    if((v->data = malloc(sizeof(uint64_t) * v->size)) == NULL)
        return -1;

    return 0;
}

int vector_insert(void *vector, uint64_t value){
    vector64_t *v = vector;

    if(vector == NULL)
        return -1;

    if(vector_isfull(vector)){
        v->size *= 2;
        if((v->data = realloc(v->data, v->size * sizeof(uint64_t))) == NULL)
            return -1;
    }

    v->data[v->next++] = value;
    return 0;
}

int vector_isfull(void *vector){
    vector64_t *v = vector;
    if(vector == NULL)
        return -1;

    return v->next == v->size;
}

int vector_delete(void *vector, uint64_t index){
    vector64_t *v = vector;
    if(vector == NULL || v->next <= index)
        return -1;

    memmove(v->data + index, v->data + index + 1, (v->next - (index + 1)) * sizeof(uint64_t));
    v->next--;
    return 0;
}


int vector_print(const void *vector){
    const vector64_t *v = vector;

    if(vector == NULL)
        return -1;

    fprintf(stdout, "\e[1;4mVector\e[0m\nnext: %lu\nsize: %lu\n<", v->next, v->size);
    for(int i = 0; i < v->next - 1; i++)
        fprintf(stdout, "%lu\t", v->data[i]);
    fprintf(stdout, "%lu>\n", v->data[v->next-1]);

    return 0;
}


int vector_free(void *vector){
    vector64_t *v = vector;
    if(vector == NULL)
        return -1;

    v->size = v->next = 0;
    reset(v->data);
    return 0;
}

int string_rename(char **string, const char *name){
    if(string == NULL || name == NULL)
        return -1;

    if(*string == NULL || strcmp(*string, name) != 0){

        // if there is not enough space, allocate more
        if(*string == NULL || strlen(*string) < strlen(name))
            *string = realloc(*string, strlen(name) + 1);

        // update name
        strcpy(*string, name);
    }

    return 0;
}

// count the number of occurrences of a char in a string
int string_count(char *str, char delim){
    int count;
    if(str == NULL)
        return -1;

    count = 0;
    while(str != NULL && *str != '\0'){
        str = strchr(str, delim);
        if(str != NULL){
            count++;
            str++;
        }
    }
    return count;
}

uint64_t get_chunk(const uint64_t key, const uint8_t chunk_size, const uint8_t chunk_index){
    uint64_t prefix, mask, index = (uint64_t)chunk_index, size = (uint64_t)chunk_size, shift = index * size;

    mask = (uint64_t)(1 << size) - 1;
    prefix = (key >> shift) & mask;
    return prefix;
}

uint64_t rand_bits(){
    uint8_t i;
    uint64_t num;
    for(i = 0, num = 0; i < BITS; i++)
        num += (num << 1) + (rand() % 2);
    return num;
}

// free allocated memory & set pointer to NULL
int reset(void *pointer){
    if(pointer == NULL)
        return -1;

    free(pointer);
    pointer = NULL;
    return 0;
}

int perror_return(const char *msg, int rv){
    perror(msg);
    return rv;
}

// count all new line characters, given a file stream
int64_t line_counter(FILE *fp){
    char ch;
    int64_t count;
    if(fp == NULL)
        return -1;

    rewind(fp);
    count = 0;
    while((ch = fgetc(fp)) != EOF)

        count += (ch == '\n');
    rewind(fp);
    return count;
}

// print a message and terminate process using the requested exit code
int error_return(int exit_code, const char *msg, ...){
    va_list props;
    va_start(props, msg);
    vfprintf(stderr, msg, props);
    va_end(props);
    return exit_code;
}

int fgets_crop(char *str, unsigned int len, FILE *in){

    // return -1 on error
    if(str == NULL || in == NULL)
        return -1;

    // return 0 on finished input stream
    else if(fgets(str, len, in) == NULL)
        return 0;

    // cut off newline and return 1 on success
    str[strlen(str)-1] = '\0';
    return 1;
}

char* find_char(char *str, char *delims, int size){
    int i;
    char *rv;
    if(str == NULL || delims == NULL)
        return NULL;

    for(i = 0, rv = NULL; i < size && rv == NULL; i++)
        rv = strchr(str, delims[i]);

    return rv;
}

// if a single bit is set, then num-1 and num, have no overlapping bits set
int is_power_of2(uint64_t num){
    return ((num - 1) & num) == 0;
}

uint64_t nearest_power_of2(uint64_t num, uint8_t bits){
    uint8_t max_bit = bits + 1;

    // if this is a power of 2, return as is
    if(is_power_of2(num))
        return num;

    // else look for the leftmost bit set
    for(uint8_t i = 0; i < bits; i++, num >>= 1)
        if(num % 2)
            max_bit = i;

    // if num == 0 return 1
    if(max_bit == bits + 1)
        return 1;
    // else set the next of the leftmost bit set(e.g 0[1]10 -> [1]000)
    else
        return 1 << (max_bit + 1);

}

void memswap(void *a, void *b, size_t size){
    void *tmp;
    assert(a && b);

    tmp = malloc(size);
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
    free(tmp);
}

int tuple_init(void *tuple){
    tuple_t *t = tuple;
    assert(tuple);

    t->key = t->payload = 0;
    return 0;
}

void tuple_set(void *tuple, uint64_t key, uint64_t payload){
    tuple_t *t = tuple;
    assert(tuple);

    t->key = key;
    t->payload = payload;
}

int tuple_randomize(void *tpl){
    tuple_t *tuple = tpl;
    assert(tpl);

    tuple->key = rand_bits();
    tuple->payload = rand_bits();
    return 0;
}

int tuple_copy(const void *src, void *dst){
    const tuple_t *a = src;
    tuple_t *b = dst;
    assert(src && dst);

    b->key = a->key;
    b->payload = a->payload;
    return 0;
}

int tuple_swap(void *tuple_a, void *tuple_b){
    tuple_t tmp;
    assert(tuple_a && tuple_b);

    tuple_copy(tuple_a, &tmp);
    tuple_copy(tuple_b, tuple_a);
    tuple_copy(&tmp, tuple_b);

    return 0;
}

int tuple_compare(const void *tuple_a, const void *tuple_b){
    const tuple_t *this = tuple_a, *other = tuple_b;
    assert(tuple_a != NULL && tuple_b != NULL);

    if(this->key != other->key)
        return this->key > other->key ? 1 : -1;
    return 0;
}

int tuple_cmp(const void *tuple_a, const void *tuple_b){
    const tuple_t *a = tuple_a, *b = tuple_b;
    assert(tuple_a && tuple_b);

    if(a->key != b->key)
        return a->key > b->key ? 1 : -1;
    else if(a->payload != b->payload)
        return a->payload > b->payload ? 1 : -1;

    return 0;
}

int tuple_print(void *tuple){
    tuple_t *t = tuple;
    assert(tuple);

    // print all columns separated by 2 tabs
    fprintf(stdout, "%-23lu %-23lu\n", t->key, t->payload);
    return 0;
}

// UINT64_MAX (filled with 1s) is used to denote a missing value
int tuple_isempty(const void *tuple){
    const tuple_t *t = tuple;
    assert(tuple);
    return t->key == t->payload && t->key == UINT64_MAX;
}

int tuple_free(void *tuple){
    tuple_t *tpl = tuple;
    assert(tuple);

    free(tpl);
    return 0;
}

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
// insert random key - value in each tuple of this table
int table_randomize(table_t *table){
    static uint8_t i = 0;
    assert(table);

    // declare that the whole table will be filled
    table->next = table->size;

    // use current time as random number generator seed
    srand(time(NULL) << i++);
    table_map(table, tuple_randomize);

    return 0;
}

// for each tuple, set payload to row_id
int table_by_row(table_t *table){
    uint64_t i;
    assert(table);

    for(i = 0; i < table->next; i++)
        table->tuples[i].payload = i;
    return 0;
}

int table_reset(table_t *table, const char *name, const header_t *header){
    assert(table && name && header);

    return string_rename(&(table->name), name) + header_reset(&(table->header), header);
}

// reducer function for table_t, accumulating the sum of sizes for a set of tables
int table_reducer(const void *table, void *total, ...){
    const table_t *this = table;
    uint64_t *acc = total;
    assert(table && total);

    *acc = *acc + this->next;
    return 0;
}

int table_to_mx(table_t* table, matrix_t *matrix, uint64_t col, tuple_t* range, uint64_t start){
    uint64_t from, to, i;
    assert(table && matrix && range);

    // deconstruct arguments
    from = range->key;
    to = range->payload;

    for(i = from; i < to; i++)
        tuple_copy(table->tuples + i, matrix->data[i - from + start] + col);

    return 0;
}

int mx_to_table(matrix_t* matrix, table_t *table, uint64_t col, tuple_t* range, uint64_t start){
    uint64_t from, to, i;
    assert(table && matrix && range);

    // deconstruct arguments
    from = range->key;
    to = range->payload;

    for(i = from; i < to; i++)
        tuple_copy(matrix->data[i] + col, table->tuples + start + i - from);

    return 0;
}

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


uint64_t table_partition(table_t *table, const tuple_t *range){
    uint64_t start, end, i, j;
    tuple_t *pivot;

    // demand of accurate arguments
    assert(table && range);

    // deconstruct arguments
    start = range->key; end = range->payload;

    // set pivot to be the last element
    pivot = &(table->tuples[end-1]);

    // i is the crossing line between smaller and larger(or not-yet-checked) elements than pivot
    for(i = start, j = i; j < end - 1; j++){
        if(tuple_compare(&(table->tuples[j]), pivot) < 0)
            tuple_swap(&(table->tuples[i++]), &(table->tuples[j]));
    }
    tuple_swap(&(table->tuples[i]), pivot);

    return i;
}

int table_quick_sort(table_t *table, const tuple_t *range){
    uint64_t start, end;
    assert(table && range);

    // deconstruct arguments
    start = range->key; end = range->payload;
    if(start < 0 || end < 1)
        return -2;

    if(start < end-1){
        uint64_t pi = table_partition(table, &(tuple_t){start, end});
        table_quick_sort(table, &(tuple_t){start, pi});
        table_quick_sort(table, &(tuple_t){pi + 1, end});
    }

    return 0;
}


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

int rel_init_wrap(void *rel, va_list args){
    char *path = va_arg(args, char*);
    return rel_init(rel, path);
}

int rel_init(rel_t *rel, const char *path){
    int fd;
    void *start;
    struct stat buf;
    size_t data_size;

    if(rel == NULL || path == NULL)
        return -1;

    // open binary file filled with data, stored by column
    if((fd = (open(path, O_RDONLY))) < 0)
        return perror_return("open", -2);

    // get relationship metadata(rows & cols), these are needed to estimate the total mapping size
    if(read(fd, &(rel->rows), sizeof(uint64_t)) != sizeof(uint64_t)
    || read(fd, &(rel->cols), sizeof(uint64_t)) != sizeof(uint64_t)){
        return close(fd) + perror_return("read header", -3);
    }

    // get actual size of file
    if(stat(path, &buf) < 0)
        return close(fd) + perror_return("stat", -4);

    // ensure that the binary file is at least as big as (header + rows * cols)
    data_size = sizeof(uint64_t) * rel->rows * rel->cols;
    if(buf.st_size < data_size + 2 * sizeof(uint64_t))
        return close(fd) - 5;

    // map data into the virtual address space of this process
    if((start = mmap(NULL, data_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        perror_return("mmap", -4);

    // data starts right after the header
    rel->data = (uint64_t*)start + 2;

    return close(fd);
}

// print by-row a by-column stored relation
int rel_print_by_row(void *relation){
    uint64_t i, j;
    rel_t *rel = relation;
    if(relation == NULL)
        return -1;

    for(i = 0; i < rel->rows; i++){
        for(j = 0; j < rel->cols; j++)
            fprintf(stdout, "%lu\t", rel->data[rel->rows * j + i]);
        putchar('\n');
    }

    return 0;
}

int rel_reduce(const rel_t *rel, void *init, uint64_t col, int (*reducer)(void *, const void *)){
    uint64_t i;
    if(rel == NULL || reducer == NULL)
        return -1;

    for(i = rel->rows * col; i < rel->rows * (col + 1); i++)
        reducer(init, rel->data + i);

    return 0;
}

int rel_add(void *total, const void* value){
    uint64_t *ttl = total;
    const uint64_t *val = value;

    if(total == NULL || value == NULL)
        return -1;
    *ttl += *val;
    return 0;
}

int rel_list_init(list_t *list){
    char path[MAX_PATH];
    assert(list);

    list_init(list, NULL, sizeof(rel_t), rel_init_wrap, rel_print_by_row, NULL);
    while(fgets_crop(path, MAX_PATH, stdin) && strcmp(PATH_TERM, path) != 0){

        fprintf(stdout, "Reading relationship from %s...", path);
        if(list_insert(list, LIST_END, path) == 0)
            fprintf(stdout, "done\n");
    }

    return 0;
}

// initialize a header given a list of fields along with their names
int header_init(header_t *header, const field_t *fields, const uint8_t size){
    int i;
    if(header == NULL || fields == NULL || size == 0)
        return -1;

    // allocate space & initialize each field of the header
    header->size = size;
    header->fields = malloc(header->size * sizeof(field_t));
    for(i = 0; i < header->size; i++){
        header->fields[i].name = NULL;
        string_rename(&(header->fields[i].name), fields[i].name);
        header->fields[i].size = fields[i].size;
    }

    return 0;
}

// release any previous allocated memory for a certain header & reset the fields and size of it
int header_reset(header_t *old_header, const header_t *new_header){
    if(old_header == NULL || new_header == NULL)
        return -1;

    header_free(old_header);
    return header_init(old_header, new_header->fields, new_header->size);
}

int header_print(header_t *header){
    int i;
    if(header == NULL)
        return -1;

    if(header->size > 0){
        for(i = 0; i < header->size; i++)
            fprintf(stdout, "\e[1;34m%-24s\e[0m",
                header->fields[i].name == NULL ? "Unknown" : header->fields[i].name);
        fputc('\n', stdout);
    }
    return 0;
}

int header_free(header_t *header){
    int i;
    if(header == NULL)
        return -1;

    // free all resources allocated for each field of the header & reset size to 0
    for(i = 0; i < header->size; i++)
        reset(header->fields[i].name);
    reset(header->fields);
    header->size = 0;
    return 0;
}

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


uint64_t matrix_partition(matrix_t *matrix, uint64_t column, const tuple_t *range){
    uint64_t start, end, i, j;
    tuple_t *pivot;

    // demand of accurate arguments
    assert(matrix && range);

    // deconstruct arguments
    start = range->key; end = range->payload;

    // set pivot to be the last element
    pivot = &(matrix->data[end-1][column]);

    // i is the crossing line between smaller and larger(or not-yet-checked) elements than pivot
    for(i = start, j = i; j < end - 1; j++){
        if(tuple_compare(matrix->data[j] + column, pivot) < 0)
            tuple_swap(matrix->data[i++] + column, matrix->data[j] + column);
    }
    tuple_swap(matrix->data[i] + column, pivot);

    return i;
}

int matrix_qsort(matrix_t *matrix, uint64_t col, const tuple_t *range){
    uint64_t start, end;
    assert(matrix && range);

    // deconstruct arguments
    start = range->key; end = range->payload;
    if(start < 0 || end < 1)
        return -2;

    if(start < end-1){
        uint64_t pi = matrix_partition(matrix, col, &(tuple_t){start, end});
        matrix_qsort(matrix, col, &(tuple_t){start, pi});
        matrix_qsort(matrix, col, &(tuple_t){pi + 1, end});
    }

    return 0;
}

// insert random key - value in each tuple of all columns in this matrix
int matrix_randomize(matrix_t *matrix, uint64_t rows, uint64_t max){
    static uint8_t i = 0;
    assert(matrix);

    // use current time as random number generator seed
    srand(time(NULL) << i++);

    for(int r = 0; r < rows; r++){
        for(int c = 0; c < matrix->cols; c++)
            matrix_set(matrix, &(tuple_t){rand_bits() % max, rand_bits() % max}, r, c);
    }

    return 0;
}

// test whether a certain column of the matrix is key-sorted
int matrix_test_order(matrix_t *matrix, uint64_t col){
    uint64_t prev, i;
    assert(matrix);

    for(i = prev = 0; i < matrix->rows; prev = matrix->data[i][col].key, i++)
        if(matrix->data[i][col].key < prev)
            return i; // return index of failure
    return 0;
}

void matrix_content_swap(matrix_t *m0, matrix_t *m1){

    assert(m0 && m1);

    memswap(&(m0->rows), &(m1->rows), sizeof(uint64_t));
    memswap(&(m0->cols), &(m1->cols), sizeof(uint64_t));
    memswap(&(m0->data), &(m1->data), sizeof(tuple_t*));
}

int matrix_init(void *matrix, uint64_t cols){
    matrix_t *m = matrix;
    assert(matrix);

    m->data = NULL;
    m->rows = 0;
    m->cols = cols;

    // map each column to the # of actual column it refers to
    m->col_map = malloc(cols * sizeof(tuple_t));
    memset(m->col_map, UINT32_MAX, cols * sizeof(tuple_t)); // fill with 1s
    return 0;
}

// add rows to a matrix, filled with -1s indicating empty spots
void matrix_resize(void *matrix, int64_t rows){
    uint64_t i;
    matrix_t *m = matrix;
    assert(matrix);

    // if this is a shrinking
    if(rows < 0){
        for(uint64_t i = m->rows + rows; i < m->rows; i++)
            reset(m->data[i]);
    }

    // update matrix
    m->rows += rows;
    m->data = realloc(m->data, m->rows * sizeof(tuple_t*));

    // fill added rows with -1s
    for(i = m->rows - rows; i < m->rows; i++){
        m->data[i] = malloc(m->cols * sizeof(tuple_t));
        memset(m->data[i], UINT32_MAX, m->cols * sizeof(tuple_t));
    }
}

void matrix_create_space(void *matrix, uint64_t row){
    matrix_t *m = matrix;
    uint64_t capacity_required;
    assert(matrix);

    capacity_required = nearest_power_of2(row + 1, BITS);
    if(m->rows == 0) // if matrix is empty
        matrix_resize(matrix, MATRIX_INITIAL_CAPACITY);
    else if(capacity_required > m->rows) // double the # of rows, if there aren't enough
        matrix_resize(matrix, capacity_required - m->rows);
}

int matrix_set(void *matrix, tuple_t *value, uint64_t row, uint64_t col){
    matrix_t *m = matrix;
    assert(matrix);
    if(col > m->cols-1)
        return -1;

    matrix_create_space(matrix, row);
    return tuple_copy(value, &(m->data[row][col]));
}

int matrix_setrow(void *matrix, tuple_t *row, uint64_t index){
    matrix_t *m = matrix;
    assert(matrix);

    matrix_create_space(matrix, index);
    memcpy(m->data[index], row, sizeof(tuple_t) * m->cols);
    return 0;
}

tuple_t * matrix_get(void *matrix, uint64_t i, uint64_t j){
    matrix_t *m = matrix;
    assert(matrix && i < m->rows && j < m->cols);
    return m->data[i] + j;
}

int matrix_empty_row(void *matrix, uint64_t row){
    int i;
    matrix_t *m = matrix;
    assert(matrix);

    if(row >= m->rows)
        return -1;

    for(i = 0; i < m->cols && tuple_isempty(m->data[row] + i); i++);
    return i == m->cols;
}

uint64_t matrix_finalize(void *matrix){
    matrix_t *m = matrix;
    uint64_t i;
    assert(matrix);

    // release consecutive bottom-most empty rows
    for(i = m->rows-1; i >= 0 && matrix_empty_row(matrix, i) > 0; i--);
    if(i < m->rows - 1)
        matrix_resize(matrix, i + 1 - m->rows);

    return m->rows - (i + 1); // the number of rows released
}

void matrix_print(void *matrix){
    uint64_t i;
    matrix_t *m = matrix;
    assert(matrix);

    // display meta-data
    fprintf(stdout,
            "\e[1;4mMatrix\e[0m\n * rows: %lu\n * cols: %lu\n * col_map: [ ",
            m->rows, m->cols);
    for(i = 0; i < m->cols; i++)
        fprintf(stdout, " (%ld, %ld) ", m->col_map[i].key, m->col_map[i].payload);

    // display actual data
    fprintf(stdout, "]\n * content\n");
    for(i = 0; i < m->rows && !matrix_empty_row(matrix, i); i++)
        matrix_rowprint(matrix, i);

    if(i < m->rows)
        fprintf(stdout, " \e[1;4m%lu\e[0m empty row(s) follow...\n", m->rows - i);
}

void matrix_rowprint(void *matrix, uint64_t row){
    matrix_t *m = matrix;
    assert(matrix && row < m->rows);

    for(uint64_t j = 0; j < m->cols; j++)
        fprintf(stdout, "(%-6ld, %6ld)\t", m->data[row][j].key, m->data[row][j].payload);
    fputc('\n', stdout);
}

void matrix_free(void *matrix){
    uint64_t i;
    matrix_t *m = matrix;
    assert(matrix);

    for(i = 0; i < m->rows; i++)
        reset(m->data[i]);
    reset(m->data);
    reset(m->col_map);
}

int matrix_isempty(void *matrix){
    matrix_t *m = matrix;
    assert(matrix);
    return m->rows == 0;
}

uint64_t matrix_col_size(void *matrix, uint64_t col){
    uint64_t i;
    matrix_t *m = matrix;
    assert(matrix);

    for(i = 0; i < m->rows && !tuple_isempty(matrix_get(matrix, i, col)); i++);
    return i;
}

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


// construct a list, given a set of type-specific methods
int list_init(list_t *list, const char* name, const size_t node_size,
             int (*init)(void *, va_list), // constructor
             int (*data_print)(void*), int (*data_free)(void*)){
    assert(list && node_size > 0);

    list->length = 0;
    list->head = list->tail = NULL;
    list->node_size = node_size;

    list->init = init;
    list->print = data_print;
    list->free_data = data_free;

    list->name = NULL;
    if(name != NULL)
        return string_rename(&(list->name), name);

    return 0;
}

// insert an item at the end of the list, initialize it's value using function 'init', which was passed as a parameter
int list_insert(list_t *list, uint8_t side, ...){
    va_list props;
    node_t* node;
    assert(list && list->init);

    // create a new node
    va_start(props, side);
    if((node = malloc(sizeof(node_t))) == NULL || (node->data = malloc(list->node_size)) == NULL) {
        perror("list_insert - malloc");
        return -2;
    }

    // initialize the new node
    list->init(node->data, props);
    va_end(props);

    // insert node in the appropriate position
    return side == LIST_START ? list_add_start(list, node) : list_add_end(list, node);
}

int list_add_start(list_t *list, node_t *node){
    assert(list && node);
    node->next = list->head;
    node->prev = list->tail;

    // link with other nodes
    if(list->head != NULL)
        list->head->prev = node;
    else
        list->tail = node;
    list->head = node;
    list->length++;
    return 0;
}


int list_add_end(list_t *list, node_t *node){
    assert(list && node);
    node->next = NULL;
    node->prev = list->tail;

    // link with other nodes
    if(list->tail != NULL)
        list->tail->next = node;
    else
        list->head = node;
    list->tail = node;
    list->length++;
    return 0;
}

int list_is_empty(const list_t *list){
    assert(list);
    return list->head == NULL;
}

// print all nodes of a list
int list_print(void *list){
    list_t *l = list;
    node_t* parser;
    assert(list && l->print);

    fprintf(stdout, "\n\e[1;4m%s - length: %lu\e[0m\n", l->name, l->length);
    for(parser = l->head; parser != NULL; parser = parser->next)
        l->print(parser->data);
    return 0;
}

// free all nodes of a list
int list_free(void* l){
    list_t *list = l;
    node_t *parser, *temp;
    assert(list);

    for(parser = list->head; parser != NULL; parser = temp){
        temp = parser->next;
        if(list->free_data != NULL)
            list->free_data(parser->data);

        reset(parser->data);
        reset(parser);
    }
    reset(list->name);
    return 0;
}

// return a pointer to the payload of the index-th list node
void *list_get(list_t *list, unsigned int index){
    node_t *node;
    unsigned int i;
    assert(list && index < list->length);

    for(i = 0, node = list->head; node != NULL && i != index; node = node->next, i++);

    return node != NULL ? node->data : NULL;
}

int list_foreach(list_t *list, int (*callback)(void *, job_scheduler *), job_scheduler *js){
    assert(list && callback);

    for(node_t *i = list->head; i != NULL; i = i->next)
        callback(i->data, js);
    return 0;
}

int list_foreach_query(list_t *list, job_scheduler *js){
    assert(list);

    query_job *qj = malloc(sizeof(query_job));
    for(node_t *i = list->head; i != NULL; i = i->next) {
        query_job_init(qj, (query_t *)i->data, js);
        job_scheduler_schedule(js, qj);
    }

        // query_exec(i->data, js);
    return 0;
}

int list_reduce(const list_t *list, void *initial, int (*reducer)(const void*, void*, ...)){
    node_t *i;
    uint64_t index;
    assert(list && reducer);

    for(index = 0, i = list->head; i != NULL; index++, i = i->next)
        reducer(i->data, initial, index, list);

    return 0;
}

void list_sort(void *list, int (*cmp)(const void *, const void *)){
    list_t *l = list;
    assert(list && cmp && l->head && l->length > 1);

    // sort and get new head
    l->head = nodelist_mergesort(l->head, cmp);

    // get new tail
    for(l->tail = l->head; l->tail->next != NULL; l->tail = l->tail->next);
}

node_t *nodelist_mergesort(node_t *head, int (*cmp)(const void *, const void *)){
    node_t *i, *j = head;
    int count;
    assert(head && cmp);

    // get the node right after the middle
    for(i = j = head, count = 0; j != NULL; i = i->next, count++)
         j = j->next != NULL ? j->next->next : j->next; // fast pointer jumps nodes at a time


    if(i == j)  // one element
        return head;
    else if(i->next != j || count > 1){  // more than 2 elements

        // split list in the middle
        i->prev->next = NULL;
        i->prev = NULL;

        // sort halves
        head = nodelist_mergesort(head, cmp);
        i = nodelist_mergesort(i, cmp);

        // merge halves
        return nodelist_merge(head, i, cmp);
    }
    // base case: 2 elements, swap if 2nd is smaller than the 1st
    else if(cmp(head->data, i->data) > 0){
        head->next = i->prev = NULL;
        head->prev = i;
        i->next = head;
        return i;
    }// else return as is
    else return head;
}

node_t *nodelist_merge(node_t *n0, node_t *n1, int (*cmp)(const void *, const void *)){
    node_t *head, *prev;
    assert(n0 && n1);

    // head starts at the smallest element
    if(cmp(n0->data, n1->data) < 0){
        prev = head = n0;
        n0 = n0->next;
    }
    else{
        prev = head = n1;
        n1 = n1->next;
    }

    // put the smaller of the 2 in the result
    while(n0 != NULL && n1 != NULL){
        if(cmp(n0->data, n1->data) < 0)
            node_inresult(&prev, &n0);
        else
            node_inresult(&prev, &n1);
    }

    // put the rest of the un-finished list in the result
    while(n0 != NULL)
        node_inresult(&prev, &n0);

    while(n1 != NULL)
        node_inresult(&prev, &n1);

    return head;
}

void node_inresult(node_t **prev, node_t **node){
    assert(prev && node && *prev && *node);
    (*node)->prev = *prev;
    *prev = (*prev)->next = *node;
    *node = (*node)->next;
}

/*
 *  predicate member methods
 */
int pred_init(void *pred, va_list props){
    assert(pred);
    pred_t *pr = pred;
    char *str = va_arg(props, char*), *right_part, filters[FILTERS] = {'=', '<', '>'}, format[] = "%lu.%lu";

    // retrieve & split on the filter symbol
    right_part = find_char(str, filters, FILTERS);
    pr->operation = *right_part;
    right_part++;

    // extract index and column of first relationship
    sscanf(str, format, &(pr->left.key), &(pr->left.payload));

    // extract index and column of right_part, it will either be a rel column or a constant value
    if(pr->operation != '='){
        pr->right.key = atoi(right_part);
        pr->right.payload = UINT64_MAX;
    }

    else if(sscanf(right_part, format, &(pr->right.key), &(pr->right.payload)) != 2)
        pr->right.payload = UINT64_MAX;

    return 0;
}

int pred_cmp(const void *pred_a, const void *pred_b){
    const pred_t *a = pred_a, *b = pred_b;
    const tuple_t *a_min, *a_max, *b_min, *b_max;
    assert(pred_a && pred_b);

    // get min & max relations of the left predicate
    a_min = a->left.key < a->right.key ? &(a->left) : &(a->right);
    a_max = a->left.key < a->right.key ? &(a->right) : &(a->left);

    // get min & max relations of the right predicate
    b_min = b->left.key < b->right.key ? &(b->left) : &(b->right);
    b_max = b->left.key < b->right.key ? &(b->right) : &(b->left);

    // compare minimum relations first, based on the relation & then column indexes
    if(tuple_cmp(a_min, b_min) != 0)
        return tuple_cmp(a_min, b_min);
    // if equal, compare maximum
    else
        return tuple_cmp(a_max, b_max);

    return 0;
}


int pred_print(void *pred){
    pred_t *p = pred;

    assert(pred);
    fprintf(stdout, "\e[4m\tOperation \e[0;1;32m %c \e[0;4mbetween:\e[0m\t %lu.%lu - %lu",
            p->operation, p->left.key, p->left.payload, p->right.key);

    if(is_pred_join(p))
        fprintf(stdout, ".%lu\n", p->right.payload);
    else
        fputc('\n', stdout);
    return 0;
}

/*
 * projection member methods
 */
int proj_init(void *proj, va_list props){
    tuple_t *p = proj;

    assert(proj);
    char *str = va_arg(props, char*);
    return sscanf(str, "%lu.%lu", &(p->key), &(p->payload)) == 2 ? 0 : -1;
}

int proj_print(void *proj){
    assert(proj);
    tuple_t *p = proj;
    fprintf(stdout, "\t%lu.%lu\n", p->key, p->payload);
    return 0;
}

/*
 * query member methods
 */
int query_init(void *query, va_list props){
    query_t *q = query;
    char *info = va_arg(props, char*), rels[MAX_QUERY], preds[MAX_QUERY], projs[MAX_QUERY], *tmp;
    list_t *list = va_arg(props, list_t*);
    int i;
    assert(query);

    // deconstruct query
    sscanf(info, "%[^|\n]|%[^|\n]|%[^|\n]", rels, preds, projs);

    // create a relationship pointer vector holding a pointer to each relationship included in this query
    q->total = string_count(rels, REL_DELIM[0]) + 1;
    q->rels = malloc(sizeof(rel_t*) * q->total);
    for(i = 0, tmp = strtok(rels, REL_DELIM); tmp != NULL; tmp = strtok(NULL, REL_DELIM), i++)
        q->rels[i] = (rel_t*)list_get(list, atoi(tmp));

    // insert predicates into the list to be executed, filters go first
    list_init(&(q->preds), "Predicates", sizeof(pred_t), pred_init, pred_print, NULL);
    for(tmp = strtok(preds, PRED_DELIM); tmp != NULL; tmp = strtok(NULL, PRED_DELIM))
        list_insert(&(q->preds), is_join(tmp) ? LIST_END : LIST_START, tmp);

    // insert predicates into the list to be executed, filters go first
    list_init(&(q->projs), "Projections", sizeof(pred_t), proj_init, proj_print, NULL);
    for(tmp = strtok(projs, PROJ_DELIM); tmp != NULL; tmp = strtok(NULL, PROJ_DELIM))
        list_insert(&(q->projs), LIST_END, tmp);
    return 0;
}

int query_print(void *query){
    query_t *q = query;
    assert(query);

    fprintf(stdout, "\e[1;4mQuery\e[0m");
    list_print(&(q->preds));
    return list_print(&(q->projs));
}

int query_destroy(void *query){
    query_t *q = query;
    assert(query);
    return reset(q->rels) + list_free(&(q->preds)) + list_free(&(q->projs));
}

/*
 * utilities
 */

int is_join(const char* query){
    char *delim;

    if(query == NULL)
        return -1;

    delim = strchr(query, '=');
    return delim != NULL && strchr(delim, '.') != NULL && query[0] != delim[0];
}

int is_pred_join(const pred_t *pred){
    assert(pred);
    return (pred->operation == '=') && pred->left.key != pred->right.key && pred->right.payload != UINT64_MAX;
}


// query_job

queueType *queue_pop(queue_t *q) {
	if ( queue_empty(q) == 0 ){
		queueType *element = q->buffer[q->front];
		q->front = ( q->front + 1 ) % q->size;
		return element;
	}
	return NULL;
}

int query_job_run(query_job *qj) {
    query_exec(qj->query);
    return 0;
}

void query_job_init(query_job *qj, query_t *query, void *js) {
    qj->query = query;
    qj->js = js;
}


void queue_create(queue_t *q, int size) {
	q->front = 0;
	q->back = 0;
	q->size = size;
	q->buffer = malloc(size * sizeof(queueType *));
}

void queue_destroy(queue_t *q) {
	printf("DESTROY\n");
	free(q->buffer);
}

int queue_empty(queue_t *q) {
	return ( q->front == q->back );
}

int queue_full(queue_t *q) {
	int neo_back = (q->back+1) % q->size;
	if (neo_back == q->front )
		return 1;
	else
		return 0;
}

int queue_push(queue_t *q, queueType *element) {
	if ( queue_full(q) == 0 ){
		q->buffer[q->back] = element;
		q->back = ( q->back + 1 ) % q->size;
		return 1;
	}
	return 0;
}

/* Start routine */
void *jobThreadWork(void *arg) {
    job_scheduler *js = (job_scheduler *) arg;
    job_scheduler_threadWork(js, NULL);
    return NULL;
}

/*
 * Getting job from queue and running it until:
 * 1) Queue is empty
 * 2) Barrier is called (bar)
 * 3) Work is done
 */
void job_scheduler_threadWork(job_scheduler *js, void *arg) {
    while (1) {
        if (pthread_mutex_lock(&(js->queueLock)) != 0) {
            fprintf(stderr, "%s\n", "lock");
            exit(EXIT_FAILURE);
        }

        while (queue_empty(&(js->q)) && !js->done && !js->bar) {
            pthread_cond_wait(&(js->cond_nonempty), &(js->queueLock));
        }

        query_job *job;
        if (queue_empty(&(js->q))) {
            job = NULL;
        } else {
            job = queue_pop(&(js->q));
        }
        if (queue_empty(&(js->q))) {
            // if queue has emptied, signal barrier
            pthread_cond_signal(&(js->cond_empty));
        }

        if (pthread_mutex_unlock(&(js->queueLock)) != 0) {
            fprintf(stderr, "%s\n", "unlock");
            exit(EXIT_FAILURE);
        }

        if (job != NULL) {
            query_job_run(job);
            // job->init(arg);
            // job->run();
            free(job);
        } else {
            if (js->done)
                return;
        }
        if (js->bar) {
            // wait until all have reached this point and bar is false again
            pthread_barrier_wait(&(js->pbar));
            pthread_barrier_wait(&(js->pbar));
        }
    }
}

/* Initialize JobScheduler info */
char job_scheduler_init(job_scheduler *js) {
    js->bar = 0;
    js->done = 0;
    queue_create(&(js->q), 100);

    pthread_mutex_init(&(js->queueLock), NULL);
    pthread_cond_init(&(js->cond_nonempty), NULL);
    pthread_cond_init(&(js->cond_empty), NULL);
    pthread_barrier_init(&(js->pbar), NULL, (unsigned) NUM_OF_THREADS + 1);

    js->num_of_threads = NUM_OF_THREADS;
    js->threads = malloc(sizeof(pthread_t)*NUM_OF_THREADS);
    for (size_t i = 0; i < js->num_of_threads; i++) {
        pthread_create(js->threads + i, NULL, jobThreadWork, js);
    }
    return 1;
}

/* Destroy JobScheduler info */
char job_scheduler_destroy(job_scheduler *js) {
    pthread_mutex_destroy(&(js->queueLock));
    pthread_cond_destroy(&(js->cond_nonempty));
    pthread_cond_destroy(&(js->cond_empty));
    pthread_barrier_destroy(&(js->pbar));

    free(js->threads);
    return 1;
}

/* If barrier is called, wait until queue is empty and enable barrier.
 * Wake threads to finish any remaining jobs.
 * Wait until all jobs are done, then disable barrier.
 */
void job_scheduler_barrier(job_scheduler *js) {
    if (pthread_mutex_lock(&(js->queueLock)) != 0) {
        fprintf(stderr, "%s\n", "lock");
        exit(EXIT_FAILURE);
    }

    while (!queue_empty(&(js->q))) {
        pthread_cond_wait(&(js->cond_empty), &(js->queueLock));
    }
    js->bar = 1;
    if (pthread_mutex_unlock(&(js->queueLock)) != 0) {
        fprintf(stderr, "%s\n", "unlock");
        exit(EXIT_FAILURE);
    }

    pthread_cond_broadcast(&(js->cond_nonempty));
    pthread_barrier_wait(&(js->pbar));
    js->bar = 0;
    pthread_barrier_wait(&(js->pbar));     // so threadWork doesnt get stuck in wait
}

/* Appoint job in JobScheduler's queue*/
int job_scheduler_schedule(job_scheduler *js, query_job *job) {
    if (pthread_mutex_lock(&(js->queueLock)) != 0) {
        fprintf(stderr, "%s\n", "lock");
        exit(EXIT_FAILURE);
    }
    queue_push(&(js->q), job);
    pthread_cond_signal(&(js->cond_nonempty));
    if (pthread_mutex_unlock(&(js->queueLock)) != 0) {
        fprintf(stderr, "%s\n", "unlock");
        exit(EXIT_FAILURE);
    }
    return 0;
}

/* Enable done and wait until all threads finish their jobs */
void job_scheduler_stop(job_scheduler *js) {
    js->done = 1;
    pthread_cond_broadcast(&(js->cond_nonempty));
    for (size_t i = 0; i < js->num_of_threads; i++) {
        pthread_join(js->threads[i], NULL);
    }
}

int batch_exec(void *batch, job_scheduler *js){
    list_t *b = batch;
    assert(batch);

    // execute each query in the batch(filters come first, joins follow)
    return list_foreach_query(b, js);
}

int batch_list_init(void *list, va_list props){
    assert(list);
    return list_init(list, "Query list", sizeof(query_t), query_init, query_print, query_destroy);
}

int batch_list_populate(void *list, void *rel_list){
    assert(list);
    list_t *batch_list = list;
    char query_info[MAX_QUERY];
    uint8_t end_of_batch = 1;
    while(fgets_crop(query_info, MAX_QUERY, stdin)){

        // create a batch if the last query read indicated the end of batch
        if(end_of_batch == 1)
            list_insert(batch_list, LIST_END);

        // read next query
        if((end_of_batch = !strcmp(BATCH_TERM, query_info)))
            continue;

        // insert query into the query list of this batch
        list_insert(batch_list->tail->data, LIST_END, query_info, rel_list);
    }

    return 0;
}
