#include "relation.h"

int rel_add(void *total, const void* value){
    uint64_t *ttl = total;
    const uint64_t *val = value;

    if(total == NULL || value == NULL)
        return -1;
    *ttl += *val;
    return 0;
}
