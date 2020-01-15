#include "list.h"
#include "harness.h"
#include "vector.h"
#include "query.h"
#include "matrix.h"

int main(int argc, char *argv[]){
    list_t rel_list, batch_list;

    // initialize linked list of relationships & batch list which consists of queries
    rel_list_init(&rel_list);
    list_init(&batch_list, "Batch list", sizeof(list_t), batch_list_init, list_print, list_free);

    // read queries until EOF, query packages are seperated by 'F'(end of batch)
    batch_list_populate(&batch_list, &rel_list);

    // for each batch read from stdin, execute all queries
    list_foreach(&batch_list, batch_exec);

    // release resources
    list_free(&rel_list);
    list_free(&batch_list);
    return 0;
}