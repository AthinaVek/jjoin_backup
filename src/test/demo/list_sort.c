#include "list.h"
#define NUMS 10
#define MAX_NUM 100

// member methods for integer
int init(void*, va_list);
int print(void*);
int destroy(void *);
int cmp(const void*, const void*);

int main(){
    list_t list;
    node_t *i;
    int nums[NUMS], j;

    srand(time(NULL));

    // create a list of integers as well as an array of integers
    list_init(&list, "my-list", sizeof(int), init, print, NULL);
    for(int i = 0; i < NUMS; i++)
        list_insert(&list, LIST_END, nums[i] = rand() % MAX_NUM);

    // sort each of them with the appropriate method
    qsort(nums, NUMS, sizeof(int), cmp);
    list_sort(&list, cmp);

    // make sure list sort had the same result with quick-sort
    for(i = list.head, j = 0; i != NULL && (*(int*)i->data) == nums[j]; i = i->next, j++);
    if(i == NULL)
        fprintf(stdout, "\e[1;32mSuccess\e[0m: all %d integers were sorted correctly\n", NUMS);
    else 
        fprintf(stdout, "\e[1;31mFailure\e[0m: failed to sort all %d integers\n", NUMS);

    // print sorted list
    list_print(&list);
    return list_free(&list);
}

int init(void* elem, va_list props){
    int *e = elem;
    assert(elem);
    *e = va_arg(props, int);
    return 0;
}

int print(void* integer){
    fprintf(stdout, "%d ", (*((int*)integer)));
    return 0;
}

int cmp(const void* num_a, const void* num_b){
    const int *a = num_a, *b = num_b;
    assert(num_a && num_b);
    return *a - *b;
}

