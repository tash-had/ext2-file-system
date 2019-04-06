#include "common.h"
#include "utils.h"
#include "path_utils.h"

/**
 * #include <time.h>
 * inode.i_dtime = time(NULL)
*/

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
    if (path[strlen(path)-1] == '/'){
        perror("Must not be a directory");
        return ENOENT;
    }
    struct ext2_group_desc *gd = get_group_desc();
    struct ext2_inode *inode_table = get_inode_table();

    PathData_t *path_data = split_path(path, NULL);
    int parent_inode = get_parent_inode(path_data);
    
    if (!new_file_exists(parent_inode, path_data, EXT2_FT_REG_FILE)){
        return ENOENT;
    }
    
    update rec len of prev dir ext2_dir_entry
    deallocate block
    dec link count
    if links count now zero,
        dealloc inode
        dealloc BLOCK
        update time_t


}