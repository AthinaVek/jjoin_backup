#ifndef define_H_
#define define_H_
    #include <stdio.h>
    #include <stdlib.h> // malloc, (s)rand etc.
    #include <stdint.h>
    #include <string.h>
    #include <stdarg.h>
    #include <inttypes.h>
    #include <time.h>
    #include <math.h>
    #include <assert.h>

    // sorting operation definitions
    #define BITS 64             // size of our data(64-bit unsigned integers)
    #define CHUNK_SIZE 8       
    #define FIRST_CHUNK  ((BITS/CHUNK_SIZE)-1)
    #define MAX_BUCKET_SIZE (BITS * 1024)

    // random data generation definitions
    #define DEFAULT_TABLE_SIZE 5
    #define TABLES 2

    // merge-join operation definitions
    #define JOIN_FACTOR 0.1
    #define PRECISION 100
    #define MAX_TABLE_SIZE (1024 * 1024)
    #define OUT "out.csv"
    #define LOG "log.txt"

    // utilities
    #define MAX(a,b) ((a) > (b) ? (a) : (b))

#endif
