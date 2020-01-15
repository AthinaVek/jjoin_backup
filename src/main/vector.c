#include "vector.h"
#include "utils.h"

int vector_init(void *vector){
    vector64_t *v = vector;
    if(vector == NULL)
        return -1;

    v->next = 0;
    v->size = VECTOR_STARTING_SIZE;

    if((v->data = malloc(sizeof(uint64_t) * v->size)) == NULL) 
        return -1;
    
    return 0;
}

int vector_insert(void *vector, uint64_t value){
    vector64_t *v = vector;

    if(vector == NULL)
        return -1; 

    if(vector_isfull(vector)){
        v->size *= 2;
        if((v->data = realloc(v->data, v->size * sizeof(uint64_t))) == NULL)
            return -1;
    }

    v->data[v->next++] = value;
    return 0;
}

int vector_isfull(void *vector){
    vector64_t *v = vector;
    if(vector == NULL)
        return -1;
    
    return v->next == v->size;
}

int vector_delete(void *vector, uint64_t index){
    vector64_t *v = vector;
    if(vector == NULL || v->next <= index)
        return -1;

    memmove(v->data + index, v->data + index + 1, (v->next - (index + 1)) * sizeof(uint64_t));
    v->next--;
    return 0;
}


int vector_print(const void *vector){
    const vector64_t *v = vector;

    if(vector == NULL)
        return -1;
    
    fprintf(stdout, "\e[1;4mVector\e[0m\nnext: %lu\nsize: %lu\n<", v->next, v->size);
    for(int i = 0; i < v->next - 1; i++)
        fprintf(stdout, "%lu\t", v->data[i]);
    fprintf(stdout, "%lu>\n", v->data[v->next-1]);
    
    return 0;
}


int vector_free(void *vector){
    vector64_t *v = vector;
    if(vector == NULL)
        return -1;

    v->size = v->next = 0;
    reset(v->data);
    return 0;
}