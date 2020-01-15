#ifndef tuple_H_
#define tuple_H_
    #include "define.h"
    #include "utils.h"

    // data structures definitions
    typedef struct tuple{
        uint64_t key;
        uint64_t payload;
    }tuple_t; 

    // declarations of member functions
    int tuple_init(void *);
    void tuple_set(void*, uint64_t, uint64_t);
    int tuple_free(void *);
    int tuple_copy(const void*, void*);
    int tuple_swap(void*, void*);
    int tuple_compare(const void*, const void*);
    int tuple_print(void *);
    int tuple_isempty(const void *);
    int tuple_randomize(void *);
    int tuple_cmp(const void *, const void*);

#endif