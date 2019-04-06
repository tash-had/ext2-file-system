#include "common.h"
#include "utils.h"
#include "path_utils.h"

int main(int argc, char **argv) {
    if (argc != 4 && argc != 5) {
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

    /**
     * TODO
     * - Do error checking
     */
    struct ext2_inode *inode_table = get_inode_table();
    PathData_t *src = split_path(src_path, NULL);
    PathData_t *dst;

    if (dest_path[strlen(dest_path)-1] == '/'){
        dst = split_path(dest_path, src->file_name);
    }
    else
    {
        dst= split_path(dest_path, NULL);  
    }
    int src_inode_num = get_inode_with_path(src);
    
    int dest_parent_inode = get_parent_inode(dst);
    if (dest_parent_inode < 0){
        return ENOENT;
    }

    if (!sym_link) {
        // initialize new inode
        if (new_file_exists(dest_parent_inode, src, EXT2_FT_REG_FILE)){
            return EEXIST;
        }
        if (inode_table[dest_parent_inode-1].i_mode & EXT2_S_IFDIR){
            return EISDIR;
        }
        int file_added = add_file_to_parent(dest_parent_inode, src_inode_num, dst->file_name, EXT2_FT_REG_FILE);
        if (file_added){
            return ENOSPC;
        }
        (&inode_table[src_inode_num-1])->i_links_count += 1;
    } else {
        /**
         * TODO
         * - Stop repeating code below
         */

        int free_block = allocate_next_free(BLOCK);
        int free_inode = allocate_next_free(INODE);
        if (free_block == -1 || free_inode == -1) {
            perror("No space");
            return ENOSPC;
        }
        init_inode(EXT2_FT_SYMLINK, free_inode, free_block);
        int file_added = add_file_to_parent(dest_parent_inode, free_inode, dst->file_name, EXT2_FT_SYMLINK);
        if (file_added){
            return ENOSPC;
        }

        /**
         * TODO
         *  - account for paths that need multiple blocks to store data
         */
        unsigned char *next_block = (unsigned char *) (disk + EXT2_BLOCK_SIZE * free_block);
        memcpy(next_block, src_path, strlen(src_path+1));
        (&inode_table[free_inode-1])->i_blocks += 2;
        (&inode_table[free_inode-1])->i_size = strlen(src_path);
        
    }


}
