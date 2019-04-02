#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "ext2.h"
#include "utils.c"

int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <image file name> <path>\n", argv[0]);
        exit(1);
    }
    init_disk(argv[1]);

    char *path = argv[2];
    if (path[0] != '/'){
        perror('Not absolute path');
        return ENOENT;
    }
    struct ext2_group_desc *gd = get_group_desc();
    struct ext2_inode *inode_table = get_inode_table();

    PathData_t *path_data = split_path(path);
    int parent_inode = get_parent_inode(path_data);

    if (new_dir_exists(parent_inode, path_data)) {
        return EEXIST;
    }

    if (parent_inode == -1) {
        return ENOENT;
    }

    int free_block = allocate_next_free(BLOCK);
    int free_inode = allocate_next_free(INODE);
    if (free_block == -1 || free_inode == -1){
        perror("No space");
        return ENOSPC;
    }


    /**
     * TODO
     * - Free path_list (see free helper in path_utils.h)
     * - Free path_data struct (see free helper in path_utils.h)
     */


}