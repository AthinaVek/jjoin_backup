#include <list.h>
#include <table.h>
#include <header.h>

#define MATCHING_RECORDS 20
#define MIN_TABLE_SIZE 1
#define TABLE_NAME "Table"

int main(){
    list_t list;
    list_init(&list, "Result list", sizeof(table_t), table_init_wrap, table_print, table_free);

    tuple_t tuple;
    char *name;
    header_t header = {(field_t[]){{"Row_id-0", sizeof(uint64_t)}, {"Row_id-1", sizeof(uint64_t)}}, 2};
    int i, j;
    for(i = 0, j = 0; i < MATCHING_RECORDS; i++){

        // create i-th table
        if(list_is_empty(&list) || table_is_full(list.tail->data)){
            name = malloc(strlen(TABLE_NAME) + 4);
            sprintf(name, "%s-%02d", TABLE_NAME, i);
            list_insert(&list, LIST_END, MIN_TABLE_SIZE + j++, name, &header);
            free(name);
        }

        // create some random tuple & insert it in the last table of the result list
        tuple_randomize(&tuple);
        table_insert(list.tail->data, &tuple);
    }

    // display results
    list_print(&list);
    uint64_t initial = 0;
    list_reduce(&list, (void*)&initial, table_reducer);
    fprintf(stdout, "\n\e[4mResult size\e[0m: \e[1m%lu\e[0m records\n", initial);

    list_free(&list);
    return 0;
}