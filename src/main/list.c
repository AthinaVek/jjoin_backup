#include "list.h"

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

int list_foreach(list_t *list, int (*callback)(void *)){
    assert(list && callback);

    for(node_t *i = list->head; i != NULL; i = i->next)
        callback(i->data);
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