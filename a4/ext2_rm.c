#include "common.h"
#include "utils.h"
#include "path_utils.h"
#include "time.h"

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
    if (path[strlen(path)-1] == '/'){
        perror("Must not be a directory");
        return ENOENT;
    }
    struct ext2_group_desc *gd = get_group_desc();
    struct ext2_inode *inode_table = get_inode_table();

    PathData_t *path_data = split_path(path, NULL);
    int parent_inode_num = get_parent_inode(path_data);
    int inode_num = get_inode_with_path(path_data);
    if (inode_num < 0){
        perror("invalid inode number");
        return ENOENT;
    }
    struct ext2_inode *inode = &inode_table[inode_num-1];

    struct ext2_inode *parent_inode = &inode_table[parent_inode_num-1];
    struct ext2_dir_entry *curr_dir = (struct ext2_dir_entry *)(disk +
                                                                EXT2_BLOCK_SIZE*parent_inode->i_block[0]);
    int traversed_len = 0;
    int rec_len = 0;
    int inode_index = 0;
    int inode_rec_len;

    while (traversed_len < EXT2_BLOCK_SIZE){
        curr_dir = (void *)curr_dir + rec_len;
        if (strcmp(curr_dir->name, path_data->file_name) == 0 && curr_dir->inode != 0){
            if (curr_dir->file_type == EXT2_FT_DIR){
                perror("cannot delete a directory");
                return ENOENT;
            }
            inode_rec_len = curr_dir->rec_len;
            break;
        }
        rec_len = curr_dir->rec_len;
        traversed_len += rec_len;
        inode_index++;
    }
    // update rec len of prev dir ent
    traversed_len = 0;
    rec_len = 0;
    curr_dir = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE*parent_inode->i_block[0]);
    int i = 0;
    while (traversed_len < EXT2_BLOCK_SIZE){
        curr_dir = (void *)curr_dir + rec_len;
        if (i == inode_index - 1){
            curr_dir->rec_len += inode_rec_len;
            break;
        }
        rec_len = curr_dir->rec_len;
        traversed_len += rec_len;
        i++;
    }

    unsigned char *block_bitmap = get_block_bitmap();
    inode->i_links_count--;
    if (inode->i_links_count == 0){
        //remove inode if no links
        int block_num;
        for (int j = 0; j < 15; j++){
            block_num = inode->i_block[j];
            if (block_num > 0){
                deallocate(block_num, BLOCK);
            }
        }
        deallocate(inode_num, INODE);
        inode->i_dtime = time(NULL);
    }
    /** TODO
     * - account for case where we need to remove indirect blocks as well
     * 
     */
    return 0;


}