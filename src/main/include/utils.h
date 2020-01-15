#ifndef utils_H_
#define utils_H_
    #include <errno.h>
    #include "define.h"

    // bit manipulation
    uint64_t rand_bits();
    uint64_t get_chunk(const uint64_t, const uint8_t, const uint8_t);
    uint64_t nearest_power_of2(uint64_t , uint8_t);
    int is_power_of2(uint64_t);

    // memory management
    int64_t line_counter(FILE *);
    int reset(void *);
    void memswap(void *, void *, size_t);

    // string related utilities
    int string_rename(char **, const char*);
    int string_count(char *, char);
    int fgets_crop(char *, unsigned int, FILE*);
    char *find_char(char *, char *, int);

    // error handling
    int perror_return(const char*, int);
    int error_return(int, const char *, ...);

#endif