#include "common.h"
#include "utils.h"
#include "path_utils.h"


int main(int argc, char **argv) {
    if(argc != 4) {
        fprintf(stderr, "Usage: %s <image file name> <path to source file> <path to dest>\n", argv[0]);
        exit(1);
    }
    init_disk(argv[1]);

    char *src_path = argv[2];
    char *dest_path = argv[3];
    if (dest_path[0] != '/'){
        perror("Not absolute path");
        return ENOENT;
    }
    struct stat buf;
    if(lstat(src_path, &buf) != 0) {
        return ENOENT;
    }
    struct ext2_group_desc *gd = get_group_desc();
    struct ext2_inode *inode_table = get_inode_table();

    PathData_t *path_data = split_path(dest_path);
    int parent_inode = get_parent_inode(path_data);

    if (new_file_exists(parent_inode, path_data)) {
        return EEXIST;
    }

    if (parent_inode == -1) { //invalid path
        return ENOENT;
    }
}