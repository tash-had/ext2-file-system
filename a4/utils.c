#include "utils.h"

#define BLOCK 0
#define INODE 1

unsigned char *disk;

void init_disk(char *img_name) {
    int fd = open(img_name, O_RDWR);
    if(fd == -1) {
        perror("open");
        exit(1);
    }

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
}


int is_valid(unsigned char *inode_bitmap, int bit_idx){
    int mask = 1 << (bit_idx % 8);
    return inode_bitmap[bit_idx/8] & mask;
}


/**
 * Allocate next free block or inode in disk
 *
 * @param disk
 * @param type
 * @return index of inode|block in inode_bitmap|block_bitmap
 */
int allocate_next_free(int type){
    struct ext2_super_block *sb = get_super_block();
    struct ext2_group_desc *gd = get_group_desc();
    unsigned char *bitmap;
    int count;
    if (type & BLOCK){
        // next free block
        bitmap = (disk + EXT2_BLOCK_SIZE*gd->bg_block_bitmap);
        count = sb->s_blocks_count / 8;

    }

    else {
        // Next free inode
        bitmap = (disk + EXT2_BLOCK_SIZE*gd->bg_inode_bitmap);
        count = sb->s_inodes_count / 8;

    }
    for (int i= 0; i < count; i++){
        for (int j = 0; j < 8; j++){
            if (!((bitmap[i] >> j) & 1)){
                bitmap[i] |= 1<<j;
                if (type & BLOCK){
                    gd->bg_free_blocks_count--;
                    sb->s_free_blocks_count--;
                }
                else {
                    gd->bg_free_inodes_count--;
                    sb->s_free_inodes_count--;
                }

                return (i*8)+j+1;
            }
        }
    }
    return -1;
}



/**
 * Initialize an inode's default vals
 *
 * @param disk
 * @param type
 * @param inode_num
 * @param block_num
 */
void init_inode(int type, int inode_idx, int block_num){
    struct ext2_group_desc *gd = get_group_desc();
    struct ext2_inode *inode_table = get_inode_table();
    struct ext2_inode new_inode;

    new_inode.i_mode = (unsigned short) type;
    new_inode.i_uid = 0;
    new_inode.i_size = EXT2_BLOCK_SIZE;
    new_inode.i_gid = 0;
    new_inode.i_links_count = 1;
    new_inode.i_blocks = 2;
    new_inode.osd1 = 0;
    for (int i = 0; i < 15; i++){
        new_inode.i_block[i] = 0;
    }
    new_inode.i_block[0] = (unsigned  int) block_num;
    new_inode.i_generation = 0;
    new_inode.i_file_acl = 0;
    new_inode.i_dir_acl = 0;
    new_inode.i_faddr = 0;
    for (int i = 0; i < 3; i++){
        new_inode.extra[i] = 0;
    }

    inode_table[inode_idx-1] = new_inode;
}

PathData_t *split_path(char *path) {
    PathData_t *path_data = init_path_data();
    struct ext2_super_block *sb = get_super_block();
    struct ext2_group_desc *gd = get_group_desc();
    struct ext2_inode *inode_table = get_inode_table();

    char *path_part = strtok(path, "/");
    while (path_part != NULL) {
        char *next_path_part = strtok(path, "/");
        if (next_path_part == NULL) {
            // the path given is just a file name
            path_data->file_name = malloc((sizeof(char) * strlen(path_part)) + 1);
            strcpy(path_data->file_name, path_part);
        } else {
            add_path_part(path_data->path, path_part);
        }
        path_part = next_path_part;
    }

    return path_data;
}

int get_alligned_dirent_len(struct ext2_dir_entry *dir_ent) {
    int actual_rec_len = sizeof(struct ext2_dir_entry *) + dir_ent->name_len;
    int alligned_len = actual_rec_len + 4  * (actual_rec_len / 4 + 1);

    if (actual_rec_len % 4 > 0) {

    }
    return actual_rec_len;
}

/**
 *
 * @param path_data
 * @return the inode number of the *parent* of the new dir entry to be created
 */
int get_parent_inode(PathData_t *path_data) {
    struct ext2_super_block *sb = get_super_block();
    struct ext2_inode *inode_table = get_inode_table();

    int inodes_count = sb->s_inodes_count;
    int inode_index = 1;
    PathNode_t *curr_path = path_data->path;

    if (curr_path->path_part == NULL) {
        // the path is just a filename. no need to traverse through directories, just check for duplicates
        // in t
        return 2;
    }
    for (int i = 0; i < inodes_count; i++){
        if (is_valid(get_inode_map(), i) && (i == inode_index)){
            struct ext2_inode curr_inode = inode_table[i];
            struct ext2_dir_entry *curr_dir = (struct ext2_dir_entry *)(disk +
                    EXT2_BLOCK_SIZE*curr_inode.i_block[0]);

            int traversed_len = 0;
            int found = 0;
            while (traversed_len < EXT2_BLOCK_SIZE){
                char *cur_path_part = curr_path->path_part;
                if (strcmp(curr_dir->name, cur_path_part) == 0 && curr_dir->inode != 0 &&
                    (curr_dir->file_type == EXT2_FT_DIR)){
                    inode_index = curr_dir->inode - 1;

                    if (curr_path->next == NULL) {
                        return curr_dir->inode;
                    }
                    curr_path = curr_path->next;
                    found = 1;
                    break;
                }
                traversed_len += curr_dir->rec_len;
                curr_dir = (void *)curr_dir + curr_dir->rec_len;
            }
            if (!found){
                return -1;
            }
        }
    }
    return -1;

}

int new_dir_exists(int parent_inode, PathData_t *path_data){
    struct ext2_super_block *sb = get_super_block();
    struct ext2_inode *inode_table = get_inode_table();
    for (int i = 0; i < sb->s_inodes_count; i++){
        if (is_valid(get_inode_map(), i) && (i == parent_inode - 1)){
            struct ext2_inode curr_inode = inode_table[i];
            struct ext2_dir_entry *curr_dir = (struct ext2_dir_entry *)(disk +
                                                                        EXT2_BLOCK_SIZE*curr_inode.i_block[0]);
            int traversed_len = 0;
            /**
             * TODO
             * Try to remove duplicate code in this (duplicate of get_parent_inode)
             */
            while (traversed_len < EXT2_BLOCK_SIZE){
                if (strcmp(curr_dir->name, path_data->file_name) == 0 && curr_dir->inode != 0 &&
                    (curr_dir->file_type == EXT2_FT_DIR)){
                    //dir already exists, abort mission
                    return 1;
                }
                traversed_len += curr_dir->rec_len;
                curr_dir = (void *)curr_dir + curr_dir->rec_len;
            }
        }
    }
    return 0;
}

void init_dir_entry(unsigned char *disk, int block_num, int type, int inode_idx, char name[]){
    struct ext2_dir_entry *new_dir_entry = (struct ext2_dir_entry *)(disk + 
                                                         EXT2_BLOCK_SIZE*block_num);
    new_dir_entry->inode = (unsigned int) (inode_idx - 1);
    if (strlen(name) > EXT2_NAME_LEN){
        exit(1);
    }
    strncpy(new_dir_entry->name, name, strlen(name)); 
    new_dir_entry->file_type = (unsigned char) type;
    new_dir_entry->name_len = (unsigned char) strlen(name);
    new_dir_entry->rec_len = EXT2_BLOCK_SIZE;
}


struct ext2_super_block *get_super_block() {
    return (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
}

struct ext2_group_desc *get_group_desc() {
    return (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE*2);
}

struct ext2_inode *get_inode_table() {
    return (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE*get_group_desc()->bg_inode_table);
}

unsigned char *get_inode_map() {
    return (disk + EXT2_BLOCK_SIZE * get_group_desc()->bg_inode_bitmap);
}

unsigned char *get_block_bitmap() {
    return (disk + EXT2_BLOCK_SIZE * get_group_desc()->bg_block_bitmap);
}