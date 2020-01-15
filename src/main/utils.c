#include "utils.h"

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