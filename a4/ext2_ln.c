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

    int dst_inode_num = get_inode_with_path(dst);
    int src_inode_num = get_inode_with_path(src);

    /**
     * TODO
     * Create a fxn to initialize this so it's not repeated
     */
    int free_block = allocate_next_free(BLOCK);
    int free_inode = allocate_next_free(INODE);
    if (free_block == -1 || free_inode == -1){
        perror("No space");
        return ENOSPC;
    }

    if (!sym_link) {
        // initialize new inode
        init_inode(EXT2_S_IFDIR, free_inode, free_block);
        int file_added = add_file_to_parent(dst_inode_num, src_inode_num , src->file_name, EXT2_FT_REG_FILE);
        get_inode_table()[src_inode_num].i_links_count += 1;
    } else {

    }


}
