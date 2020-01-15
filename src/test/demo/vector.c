#include "vector.h"

#define ELEMS 5

int main(){
    vector64_t v;
    vector_init(&v);

    for(uint64_t i = 0; i < ELEMS; i++)
        vector_insert(&v, i*i);

    vector_print(&v);
    vector_delete(&v, 2);
    vector_print(&v);
    vector_delete(&v, 2);
    vector_print(&v);
    vector_free(&v);
    
    return 0;
}