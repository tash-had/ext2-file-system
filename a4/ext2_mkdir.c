#include "common.h"
#include "utils.h"
#include "path_utils.h"

int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <image file name> <path>\n", argv[0]);
        exit(1);
    }
    init_disk(argv[1]);

    char *path = argv[2];
    if (path[0] != '/'){
        perror("Not absolute path");
        return ENOENT;
    }
    struct ext2_group_desc *gd = get_group_desc();
    struct ext2_inode *inode_table = get_inode_table();

    PathData_t *path_data = split_path(path);
    int parent_inode = get_parent_inode(path_data);

    if (new_dir_exists(parent_inode, path_data)) {
        return EEXIST;
    }

    if (parent_inode == -1) { //invalid path
        return ENOENT;
    }

    int free_block = allocate_next_free(BLOCK);
    int free_inode = allocate_next_free(INODE);
    if (free_block == -1 || free_inode == -1){
        perror("No space");
        return ENOSPC;
    }

    // initialize new inode
    init_inode(EXT2_S_IFDIR, free_inode, free_block);
    int root_len = get_rec_len(".");
    init_dir_entry(free_block, 0, EXT2_FT_DIR, free_inode, ".", root_len); // add ./ dir
    init_dir_entry(free_block, root_len, EXT2_FT_DIR, parent_inode, "..", EXT2_BLOCK_SIZE-root_len); // add ../ dir
    int ret = add_dir_to_parent(parent_inode, free_inode, path_data->file_name);
    if (ret){
        return ENOSPC;
    }
    gd->bg_used_dirs_count++;


    /**
     * TODO
     * - Free path_list (see free helper in path_utils.h)
     * - Free path_data struct (see free helper in path_utils.h)
     */


}