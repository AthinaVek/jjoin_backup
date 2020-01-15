#include <string.h>
#include "harness.h"
#include "query.h"

int batch_exec(void *batch){
    list_t *b = batch;
    assert(batch);

    // execute each query in the batch(filters come first, joins follow)
    return list_foreach(b, query_exec);
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