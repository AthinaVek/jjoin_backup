#include "master.h"


int main(int argc, char *argv[]){
    list_t rel_list, batch_list;

    // initialize linked list of relationships & batch list which consists of queries
    rel_list_init(&rel_list);
    list_init(&batch_list, "Batch list", sizeof(list_t), batch_list_init, list_print, list_free);

    // read queries until EOF, query packages are seperated by 'F'(end of batch)
    batch_list_populate(&batch_list, &rel_list);

    job_scheduler *js = malloc(sizeof(job_scheduler));
    job_scheduler_init(js);
    // for each batch read from stdin, execute all queries
    list_foreach(&batch_list, batch_exec, js);

    // release resources
    list_free(&rel_list);
    list_free(&batch_list);
    return 0;
}
