#include "tuple.h"

int tuple_init(void *tuple){
    tuple_t *t = tuple;
    assert(tuple);

    t->key = t->payload = 0;
    return 0;
}

void tuple_set(void *tuple, uint64_t key, uint64_t payload){
    tuple_t *t = tuple;
    assert(tuple);

    t->key = key;
    t->payload = payload;
}

int tuple_randomize(void *tpl){
    tuple_t *tuple = tpl;
    assert(tpl);

    tuple->key = rand_bits();
    tuple->payload = rand_bits();
    return 0;
}

int tuple_copy(const void *src, void *dst){
    const tuple_t *a = src;
    tuple_t *b = dst;
    assert(src && dst);

    b->key = a->key;
    b->payload = a->payload;
    return 0;
}

int tuple_swap(void *tuple_a, void *tuple_b){
    tuple_t tmp;
    assert(tuple_a && tuple_b);

    tuple_copy(tuple_a, &tmp);
    tuple_copy(tuple_b, tuple_a);
    tuple_copy(&tmp, tuple_b);

    return 0;
}

int tuple_compare(const void *tuple_a, const void *tuple_b){
    const tuple_t *this = tuple_a, *other = tuple_b;
    assert(tuple_a != NULL && tuple_b != NULL);

    if(this->key != other->key)
        return this->key > other->key ? 1 : -1;
    return 0;
}

int tuple_cmp(const void *tuple_a, const void *tuple_b){
    const tuple_t *a = tuple_a, *b = tuple_b;
    assert(tuple_a && tuple_b);

    if(a->key != b->key)
        return a->key > b->key ? 1 : -1;
    else if(a->payload != b->payload)
        return a->payload > b->payload ? 1 : -1;

    return 0;
}

int tuple_print(void *tuple){
    tuple_t *t = tuple;
    assert(tuple);
    
    // print all columns separated by 2 tabs
    fprintf(stdout, "%-23lu %-23lu\n", t->key, t->payload);
    return 0;
}

// UINT64_MAX (filled with 1s) is used to denote a missing value
int tuple_isempty(const void *tuple){
    const tuple_t *t = tuple;
    assert(tuple);
    return t->key == t->payload && t->key == UINT64_MAX;
}

int tuple_free(void *tuple){
    tuple_t *tpl = tuple;
    assert(tuple);

    free(tpl);
    return 0;
}