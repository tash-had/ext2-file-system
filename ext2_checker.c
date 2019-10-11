#include "common.h"
#include "utils.h"
#include "path_utils.h"


int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <image file name>\n", argv[0]);
        exit(1);
    }
    init_disk(argv[1]);
    int total = total_inconsistencies();

    if (total > 0){
        printf("%d file system inconsistencies repaired!\n", total);
    }
    else{
        printf("No file system inconsistencies detected!\n");
    }

}