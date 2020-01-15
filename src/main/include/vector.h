#ifndef vector_H_
#define vector_H_
    #include "define.h"

    #define VECTOR_STARTING_SIZE 2

    typedef struct vector64{
        uint64_t next;
        uint64_t size;
        uint64_t *data; 
    }vector64_t;

    int vector_init(void *);
    int vector_insert(void *, uint64_t);
    int vector_delete(void *, uint64_t);
    int vector_isfull(void *);
    int vector_print(const void *);
    int vector_free(void *);

#endif