#include "common.h"
#include "utils.h"

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <image file name>\n", argv[0]);
        exit(1);
    }
    init_disk(argv[1]);
}