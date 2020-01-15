#include "query.h"
#define PREDS 3

int main(){

    list_t list;

    list_init(&list, "Predicates", sizeof(pred_t), pred_init, pred_print, NULL);
    char* preds[PREDS] = {"2.0=5.0", "5.1=1.0", "0.0=5.0"};

    for(int i = 0; i < PREDS; i++)
        list_insert(&list, LIST_END, preds[i]);

    list.head = nodelist_mergesort(list.head, pred_cmp);
    list_print(&list);
    list_free(&list);
    return 0;
}