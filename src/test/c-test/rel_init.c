#include "relation.h"

int main(int argc, char *argv[]){
    char *path_bin = "/home/kostas/Git/Repos/c/jjoin/rsrc/datasets/part2/small/r0";
    rel_t rel;

    if(argc > 1)
        path_bin = argv[1];

    if(rel_init(&rel, path_bin) < 0)
        return 1;

    rel_print_by_row(&rel);
    return 0;
}