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
    struct ext2_inode *inode_table = get_inode_table();

    PathData_t *src_path_data = split_path(src_path, NULL);
    char *src_file_name = src_path_data->file_name;
    PathData_t *dest_path_data;
    int dest_parent_inode;
    if (dest_path[strlen(dest_path)-1] == '/'){
        dest_path_data = split_path(dest_path, src_file_name);
        dest_parent_inode = get_parent_inode(dest_path_data);
    }
    else
    {
        dest_path_data = split_path(dest_path, NULL);
        dest_parent_inode = get_parent_inode(dest_path_data);   
    }
    


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
        return ENOSPC;
    }

    init_inode(EXT2_S_IFREG, free_inode, free_block);
    inode_table[free_inode-1].i_size = 0; //new empty file so set size to 0
    add_file_to_parent(dest_parent_inode, free_inode, dest_path_data->file_name, EXT2_FT_REG_FILE);

    struct ext2_inode *inode = &inode_table[free_inode - 1];
    int ret = copy_to_fs(src_file, inode, free_block);
    if (ret == 1){
        return ENOENT;
    }
    else if(ret == 2){
        return ENOSPC;
    }
}