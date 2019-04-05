#include "common.h"
#include "utils.h"
#include "path_utils.h"

int main(int argc, char **argv) {
    if(argc != 4 && argc != 5) {
        fprintf(stderr, "Usage: %s <image file name> [-s] <path to source file> <path to dest>\n", argv[0]);
        exit(1);
    }
    init_disk(argv[1]);

    char *src_path = argv[2];
    char *dest_path = argv[3];
    int sym_link = 0;

    if (argc == 5 && strcmp(src_path, "-s") == 0) {
        sym_link = 1;
        src_path = argv[3];
        dest_path = argv[4];
    }

    PathData_t *src = split_path(src_path, NULL);
    PathData_t *dst = split_path(dest_path, NULL);

    int src_parent_inode = get_parent_inode(src);
    int dst_parent_inode = get_parent_inode(dst);

    if (!sym_link) {
//        add_file_to_parent(src_parent_inode, )
    } else {

    }


}
