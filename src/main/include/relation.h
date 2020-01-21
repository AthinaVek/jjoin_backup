#ifndef relation_H_
#define relation_H_
    #include <sys/stat.h>  // stat
    #include <sys/mman.h>  // mmap
    #include <fcntl.h>     // open
    #include <unistd.h>    // close
    #include "define.h"

    typedef struct relation{
        uint64_t rows;
        uint64_t cols;
        uint64_t *data;
    }rel_t;

    // member methods
    int rel_init(rel_t *, const char *);
    int rel_init_wrap(void *, va_list);
    int rel_print_by_row(void *);
    int rel_reduce(const rel_t *, void *, uint64_t, int (*)(void *, const void *));

    // utilities
    int rel_add(void *, const void *);

#endif
