#ifndef list_H_
#define list_H_   
    #include "define.h"
    #include "utils.h"
    #include "header.h"

    #define LIST_START 0
    #define LIST_END 1

    typedef struct node{
        void *data;
        struct node *next;
        struct node *prev;
    }node_t;

    typedef struct list{

        // field type properties
        node_t* head;
        node_t* tail;
        uint64_t length; 
        char *name;  
        size_t node_size;

        // method type properties
        int (*init)(void*, va_list);
        int (*print)(void*);
        int (*free_data)(void *);
    }list_t;

    // member methods
    int list_init(list_t*, const char *, const size_t, 
                 int (*)(void*, va_list),// tableconstructor 
                 int (*)(void*), int (*)(void*));
    int list_insert(list_t*, uint8_t, ...);
    int list_add_start(list_t *, node_t *);
    int list_add_end(list_t *, node_t *);
    int list_is_empty(const list_t *);
    int list_print(void *);
    int list_free(void *);
    int list_foreach(list_t *list, int (*)(void *));
    void *list_get(list_t*, unsigned int);
    void list_sort(void *, int (*)(const void*, const void*));
    int list_reduce(const list_t *, void *, int (*)(const void*, void*, ...));
    node_t *nodelist_mergesort(node_t *, int (*)(const void *, const void *));
    node_t *nodelist_merge(node_t *, node_t *, int (*)(const void *, const void *));
    void node_inresult(node_t **, node_t **);



#endif