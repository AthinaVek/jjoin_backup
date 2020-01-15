#include "query.h"

/* 
 *  predicate member methods 
 */
int pred_init(void *pred, va_list props){
    assert(pred);
    pred_t *pr = pred;
    char *str = va_arg(props, char*), *right_part, filters[FILTERS] = {'=', '<', '>'}, format[] = "%lu.%lu";

    // retrieve & split on the filter symbol
    right_part = find_char(str, filters, FILTERS);
    pr->operation = *right_part;
    right_part++;

    // extract index and column of first relationship
    sscanf(str, format, &(pr->left.key), &(pr->left.payload));

    // extract index and column of right_part, it will either be a rel column or a constant value
    if(pr->operation != '='){
        pr->right.key = atoi(right_part);
        pr->right.payload = UINT64_MAX;
    }
        
    else if(sscanf(right_part, format, &(pr->right.key), &(pr->right.payload)) != 2)
        pr->right.payload = UINT64_MAX;

    return 0;
}

int pred_cmp(const void *pred_a, const void *pred_b){
    const pred_t *a = pred_a, *b = pred_b;
    const tuple_t *a_min, *a_max, *b_min, *b_max;
    assert(pred_a && pred_b);

    // get min & max relations of the left predicate
    a_min = a->left.key < a->right.key ? &(a->left) : &(a->right);
    a_max = a->left.key < a->right.key ? &(a->right) : &(a->left);

    // get min & max relations of the right predicate
    b_min = b->left.key < b->right.key ? &(b->left) : &(b->right);
    b_max = b->left.key < b->right.key ? &(b->right) : &(b->left); 

    // compare minimum relations first, based on the relation & then column indexes
    if(tuple_cmp(a_min, b_min) != 0)   
        return tuple_cmp(a_min, b_min);
    // if equal, compare maximum
    else    
        return tuple_cmp(a_max, b_max);

    return 0;
}


int pred_print(void *pred){
    pred_t *p = pred;

    assert(pred);
    fprintf(stdout, "\e[4m\tOperation \e[0;1;32m %c \e[0;4mbetween:\e[0m\t %lu.%lu - %lu", 
            p->operation, p->left.key, p->left.payload, p->right.key);
    
    if(is_pred_join(p))
        fprintf(stdout, ".%lu\n", p->right.payload);
    else
        fputc('\n', stdout);
    return 0;
}

/*
 * projection member methods
 */
int proj_init(void *proj, va_list props){
    tuple_t *p = proj;

    assert(proj);
    char *str = va_arg(props, char*);
    return sscanf(str, "%lu.%lu", &(p->key), &(p->payload)) == 2 ? 0 : -1;
}

int proj_print(void *proj){
    assert(proj);
    tuple_t *p = proj;
    fprintf(stdout, "\t%lu.%lu\n", p->key, p->payload);
    return 0;
}

/* 
 * query member methods
 */
int query_init(void *query, va_list props){
    query_t *q = query;
    char *info = va_arg(props, char*), rels[MAX_QUERY], preds[MAX_QUERY], projs[MAX_QUERY], *tmp;
    list_t *list = va_arg(props, list_t*);
    int i;
    assert(query);

    // deconstruct query
    sscanf(info, "%[^|\n]|%[^|\n]|%[^|\n]", rels, preds, projs);

    // create a relationship pointer vector holding a pointer to each relationship included in this query
    q->total = string_count(rels, REL_DELIM[0]) + 1;
    q->rels = malloc(sizeof(rel_t*) * q->total);
    for(i = 0, tmp = strtok(rels, REL_DELIM); tmp != NULL; tmp = strtok(NULL, REL_DELIM), i++)
        q->rels[i] = (rel_t*)list_get(list, atoi(tmp));

    // insert predicates into the list to be executed, filters go first
    list_init(&(q->preds), "Predicates", sizeof(pred_t), pred_init, pred_print, NULL);
    for(tmp = strtok(preds, PRED_DELIM); tmp != NULL; tmp = strtok(NULL, PRED_DELIM))
        list_insert(&(q->preds), is_join(tmp) ? LIST_END : LIST_START, tmp);

    // insert predicates into the list to be executed, filters go first
    list_init(&(q->projs), "Projections", sizeof(pred_t), proj_init, proj_print, NULL);
    for(tmp = strtok(projs, PROJ_DELIM); tmp != NULL; tmp = strtok(NULL, PROJ_DELIM))
        list_insert(&(q->projs), LIST_END, tmp);
    return 0;
}

int query_print(void *query){
    query_t *q = query;
    assert(query);

    fprintf(stdout, "\e[1;4mQuery\e[0m");
    list_print(&(q->preds));
    return list_print(&(q->projs));
}

int query_destroy(void *query){
    query_t *q = query;
    assert(query);
    return reset(q->rels) + list_free(&(q->preds)) + list_free(&(q->projs));
}

/*
 * utilities
 */

int is_join(const char* query){
    char *delim;

    if(query == NULL)
        return -1;

    delim = strchr(query, '=');
    return delim != NULL && strchr(delim, '.') != NULL && query[0] != delim[0];
}

int is_pred_join(const pred_t *pred){
    assert(pred);
    return (pred->operation == '=') && pred->left.key != pred->right.key && pred->right.payload != UINT64_MAX;
}
