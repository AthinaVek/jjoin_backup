
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <define.h>
#include "harness.h"
#define RELS 2
#define NAME "Rel-"

int main(int argc, char *argv[]){
    uint8_t i, j;
    uint64_t initial, size;
    tuple_t tuple;
    table_t table[RELS];
    list_t list;
    FILE *file[RELS];
    char *name;
    header_t header = {(field_t[]){{"Primary Key", sizeof(uint64_t)}, {"Row_id", sizeof(uint64_t)}}, 2};

    // require one file for each relationship
    if(argc != 1 + RELS)
        error_return(EXIT_FAILURE, "Usage: %s <rel-path_a> <rel-path_b>", argv[0]);

    // for each relationship/table
    for(i = 0; i < RELS; i++){

        // if file does not exist, close all opened until now files, free initialized tables & exit
        if((file[i] = fopen(argv[1 + i], "r")) == NULL){
            perror("fopen");
            for(j = 0; j < i; j++){
                fclose(file[j]);
                table_free(&(table[i]));
            }
            error_return(EXIT_FAILURE, "Unable to open file %s\n_exiting now...", argv[1 + i]);
        }

        // get the size of the file in order to initialize a relationship appropriately
        size = line_counter(file[i]);

        // create a name for the table & initialize it
        name = malloc(strlen(NAME) + 3);
        sprintf(name, "%s%02d", NAME, i);
        table_init(&(table[i]), size + 1, name, &header);
        reset(name);

        // insert all tuples read from the file
        while(fscanf(file[i], "%lu,%lu", &(tuple.key), &(tuple.payload)) != EOF)
            table_insert(&(table[i]), &tuple);

        // sort table
        table_sort_wrapper(&(table[i]));
    }

    // join tables
    list_init(&list, "Result", sizeof(table_t), table_init_wrap, table_print, table_free);
    join(&(table[0]), &(table[1]), &list);

    // calculate and display statistics
    initial = 0;
    list_reduce(&list, &initial, table_reducer);
    fprintf(stdout, "\e[4mSize_A\e[0m: %lu\n\e[4mSize_B\e[0m: %lu\n"\
                    "\e[4mResult size\e[0m: %lu records\n\e[4mJoin Factor\e[0m: %.2f\n",
                    table[0].next, table[1].next, initial, 
                    (float)initial / (float)(table[0].next * table[1].next));

    // clean-up
    for(i = 0; i < RELS; i++){
        fclose(file[i]);
        table_free(&(table[i]));
    }
    list_free(&list);
    return 0;
}