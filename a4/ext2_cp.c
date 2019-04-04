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
    struct ext2_group_desc *gd = get_group_desc();
    struct ext2_inode *inode_table = get_inode_table();


    PathData_t *src_path_data = split_path(src_path, NULL);
    char *src_file_name = src_path_data->file_name;

    PathData_t *dest_path_data = split_path(dest_path, src_file_name);
    int dest_parent_inode = get_parent_inode(dest_path_data);


    if (new_file_exists(dest_parent_inode, dest_path_data, EXT2_FT_REG_FILE)) {
        return EEXIST;
    }

    FILE *src_file = fopen(src_path, "rb");
    if (src_file == NULL){
        return ENOENT;
    }
    if (dest_parent_inode == -1) { //invalid path
        return ENOENT;
    }

    int free_block = allocate_next_free(BLOCK); 
    int free_inode = allocate_next_free(INODE);
    if (free_block == -1 || free_inode == -1){
        perror("No space");
        return ENOSPC;
    }

    init_inode(EXT2_S_IFREG, free_inode, free_block);
    add_file_to_parent(dest_parent_inode, free_inode, src_file_name, EXT2_FT_REG_FILE);
}