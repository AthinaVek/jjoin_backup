#include "header.h"
#include "utils.h"

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