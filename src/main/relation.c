#include "utils.h"
#include "relation.h"

int rel_init_wrap(void *rel, va_list args){
    char *path = va_arg(args, char*);
    return rel_init(rel, path);
}

int rel_init(rel_t *rel, const char *path){
    int fd;
    void *start;
    struct stat buf;
    size_t data_size;

    if(rel == NULL || path == NULL)
        return -1;

    // open binary file filled with data, stored by column
    if((fd = (open(path, O_RDONLY))) < 0)
        return perror_return("open", -2);

    // get relationship metadata(rows & cols), these are needed to estimate the total mapping size
    if(read(fd, &(rel->rows), sizeof(uint64_t)) != sizeof(uint64_t) 
    || read(fd, &(rel->cols), sizeof(uint64_t)) != sizeof(uint64_t)){
        return close(fd) + perror_return("read header", -3);
    } 

    // get actual size of file
    if(stat(path, &buf) < 0)
        return close(fd) + perror_return("stat", -4);

    // ensure that the binary file is at least as big as (header + rows * cols)
    data_size = sizeof(uint64_t) * rel->rows * rel->cols;
    if(buf.st_size < data_size + 2 * sizeof(uint64_t))
        return close(fd) - 5; 

    // map data into the virtual address space of this process
    if((start = mmap(NULL, data_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) 
        perror_return("mmap", -4);

    // data starts right after the header
    rel->data = (uint64_t*)start + 2;

    return close(fd);
}

// print by-row a by-column stored relation
int rel_print_by_row(void *relation){
    uint64_t i, j;
    rel_t *rel = relation;
    if(relation == NULL)
        return -1;

    for(i = 0; i < rel->rows; i++){
        for(j = 0; j < rel->cols; j++)
            fprintf(stdout, "%lu\t", rel->data[rel->rows * j + i]);
        putchar('\n');
    }

    return 0;
}

int rel_reduce(const rel_t *rel, void *init, uint64_t col, int (*reducer)(void *, const void *)){
    uint64_t i;
    if(rel == NULL || reducer == NULL)
        return -1;

    for(i = rel->rows * col; i < rel->rows * (col + 1); i++)
        reducer(init, rel->data + i);

    return 0;
}