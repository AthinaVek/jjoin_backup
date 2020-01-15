#ifndef header_H_
#define header_H_
    #include "define.h"

    // data structures definitions
    typedef struct field{
        char *name;
        size_t size;
    }field_t;

    typedef struct header{
        field_t *fields;
        uint8_t size;
    }header_t;

    int header_init(header_t *, const field_t*, const uint8_t);
    int header_reset(header_t *, const header_t*);
    int header_print(header_t *header);
    int header_free(header_t *header);
#endif