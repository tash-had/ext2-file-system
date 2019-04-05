#include "utils.h"
#include "path_utils.h"

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
    check_valid(disk, "disk");
}

void check_valid(void *ext2_struct, char *struct_description) {
    if (!ext2_struct || ext2_struct == NULL) {
        fprintf(stderr, "Trouble initializing %s.", struct_description);
        exit(1);
    }
}

struct ext2_super_block *get_super_block() {
    check_valid(disk, "disk");
    struct ext2_super_block *sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
    check_valid(disk, "super block");
    return sb;
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
    if (type == BLOCK){
        // next free block
        bitmap = get_block_bitmap();
        count = sb->s_blocks_count / 8;

    }
    else {
        // Next free inode
        bitmap = get_inode_map();
        count = sb->s_inodes_count / 8;

    }

    for (int i= 0; i < count; i++){
        for (int j = 0; j < 8; j++){

//            printf(" %d ", (bitmap[i] >> j) & 1 );
            if (!((bitmap[i] >> j) & 1)){
                bitmap[i] |= 1<<j;
                if (type == BLOCK){
                    gd->bg_free_blocks_count--;
                    sb->s_free_blocks_count--;
                }
                else {
                    gd->bg_free_inodes_count--;
                    sb->s_free_inodes_count--;
                }
                //returns INODE NUMBER and BLOCK NUMBER
                return (i*8)+j + 1;
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
void init_inode(int type, int inode_num, int block_num){
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

    inode_table[inode_num-1] = new_inode;
}

/**Returns inode num of parent inode
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

    if (curr_path->next == NULL) {
        // the path is just a filename. no need to traverse through directories, just check for duplicates
        // in t
        return EXT2_ROOT_INO; //root is the parent
    }
    for (int i = 0; i < inodes_count; i++){
        if (is_valid(get_inode_map(), i) && (i == inode_index)){
            struct ext2_inode curr_inode = inode_table[i];
            struct ext2_dir_entry *curr_dir = (struct ext2_dir_entry *)(disk +
                    EXT2_BLOCK_SIZE*curr_inode.i_block[0]);

            int traversed_len = 0;
            int rec_len = 0;
            int found = 0;
            while (traversed_len < EXT2_BLOCK_SIZE){
                curr_dir = (void *)curr_dir + rec_len;
                char *cur_path_part = curr_path->path_part;
                if (strcmp(curr_dir->name, cur_path_part) == 0 && curr_dir->inode != 0 &&
                    (curr_dir->file_type == EXT2_FT_DIR)){
                    inode_index = curr_dir->inode - 1;

                    if (curr_path->next->path_part == NULL) {
                        return curr_dir->inode;
                    }
                    curr_path = curr_path->next;
                    found = 1;
                    break;
                }
                rec_len = curr_dir->rec_len;
                traversed_len += rec_len;
            }
            if (!found){
                return -1;
            }
        }
    }
    return -1;

}

struct ext2_inode *get_inode(unsigned int inode_num) {

//    if (pd == NULL) {
        int inode_idx = inode_num - 1;

        if (inode_idx > get_super_block()->s_inodes_count ||
            (inode_idx != EXT2_ROOT_INO && inode_idx < get_super_block()->s_first_ino)) {
        }
        return &get_inode_table()[inode_idx];
//    }

//    }

}


//check if file to be made exists
int new_file_exists(int parent_inode, PathData_t *path_data, int type){
    struct ext2_super_block *sb = get_super_block();
    struct ext2_inode *inode_table = get_inode_table();
    for (int i = 0; i < sb->s_inodes_count; i++){
        if (is_valid(get_inode_map(), i) && (i == parent_inode - 1)){
            struct ext2_inode curr_inode = inode_table[i];
            struct ext2_dir_entry *curr_dir = (struct ext2_dir_entry *)(disk +
                                                                        EXT2_BLOCK_SIZE*curr_inode.i_block[0]);
            int traversed_len = 0;
            int rec_len = 0;
            /**
             * TODO
             * Try to remove duplicate code in this (duplicate of get_parent_inode)
             */
            while (traversed_len < EXT2_BLOCK_SIZE){
                curr_dir = (void *)curr_dir + rec_len;
                if (strcmp(curr_dir->name, path_data->file_name) == 0 && curr_dir->inode != 0 &&
                    (curr_dir->file_type == type)){
                    //dir already exists, abort mission
                    return 1;
                }
                rec_len = curr_dir->rec_len;
                traversed_len += rec_len;
                
            }
        }
    }
    return 0;
}

int get_rec_len(char *dir_ent_name){
    if (!(strlen(dir_ent_name) % 4)){
        return sizeof(struct ext2_dir_entry) + strlen(dir_ent_name);
    }
    else{
        int new_len = 4*((strlen(dir_ent_name)/4)+1); //round up to multiple of 4
        return sizeof(struct ext2_dir_entry) + new_len;
    }
}

//initializes dir entry
void init_dir_entry(int dir_block_num, int offset,  int type, int inode_num, char name[], int size){
    struct ext2_inode *inode_table = get_inode_table();
    struct ext2_dir_entry *new_dir_entry = (struct ext2_dir_entry *)(disk + 
                                                         EXT2_BLOCK_SIZE*dir_block_num + offset);
    new_dir_entry->inode = inode_num;
    if (strlen(name) > EXT2_NAME_LEN){
        exit(1);
    }
    new_dir_entry->rec_len = size;
    strncpy(new_dir_entry->name, name, strlen(name)); 
    new_dir_entry->file_type = type;
    new_dir_entry->name_len = strlen(name);
    
}

//adds new file to parent dir, adjust rec len of prev dir entry
int add_file_to_parent(int parent_inode_num, int inode_num, char name[], int type){
    struct ext2_inode *parent = get_inode((unsigned int) parent_inode_num);
    unsigned int block_num = parent->i_block[0];
    struct ext2_dir_entry *curr_dir_entry = (struct ext2_dir_entry *)(disk + 
                                                         EXT2_BLOCK_SIZE*block_num);

    struct ext2_inode *inode_table = get_inode_table();
    int total_len = 0;
    int rec_len = 0;
    int total_actual_size = 0;
    while (total_len < EXT2_BLOCK_SIZE){
        curr_dir_entry = (void *)curr_dir_entry + rec_len;
        total_actual_size += get_rec_len(curr_dir_entry->name);
        rec_len = curr_dir_entry->rec_len;
        total_len += rec_len;
    }

    if (total_actual_size + get_rec_len(name) > EXT2_BLOCK_SIZE){
        return 1;
    }

    total_len = 0;
    rec_len = 0;
    int actual_size;

    curr_dir_entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE*block_num);
    while (total_len < EXT2_BLOCK_SIZE){
        curr_dir_entry = (void *)curr_dir_entry + rec_len;
        actual_size = get_rec_len(curr_dir_entry->name);
        rec_len = curr_dir_entry->rec_len;
        if (actual_size < rec_len){
            curr_dir_entry->rec_len = actual_size;
        }
        total_len += rec_len;
    }
    if (type == EXT2_FT_DIR){
        inode_table[parent_inode_num - 1].i_links_count++;
    }
    init_dir_entry(block_num, total_actual_size, type, inode_num, name, EXT2_BLOCK_SIZE-total_actual_size);
    return 0;
}

int copy_to_fs(FILE *src, struct ext2_inode *inode, int block_num){

    unsigned char buf[EXT2_BLOCK_SIZE];
    int amt_read = 0;
    int block_ptr = 0;
    
    amt_read = fread(buf, 1, EXT2_BLOCK_SIZE, src);
    unsigned char *next_block = (unsigned char *)(disk + EXT2_BLOCK_SIZE * block_num);
    inode->i_size = amt_read;
    memcpy(next_block, buf, amt_read);

    int i = 1;
    while ((i < 12) && (amt_read = fread(buf, 1, EXT2_BLOCK_SIZE, src)) > 0){
        block_ptr = allocate_next_free(BLOCK);
        if (block_ptr < 0) {
            return 1;
        }
        next_block = (unsigned char *)(disk + EXT2_BLOCK_SIZE * block_ptr);
        inode->i_size += amt_read;
        inode->i_block[i] = block_ptr;
        inode->i_blocks += 1;
        memcpy(next_block, buf, amt_read);
        i++;
 
    }
    if ((amt_read = fread(buf, 1, EXT2_BLOCK_SIZE, src)) <= 0) {
        return 0;
    }

    //need to use indirect block as not all data was stored

    block_ptr = allocate_next_free(BLOCK);
    inode->i_block[i] = block_ptr;
    inode->i_blocks += 1;
    int *indirect_block = (int *)(disk + block_ptr * EXT2_BLOCK_SIZE);
    
    i = 0;
    while ((i < EXT2_BLOCK_SIZE) && (amt_read = fread(buf, 1, EXT2_BLOCK_SIZE, src)) > 0){
        block_ptr = allocate_next_free(BLOCK);

        if (block_ptr < 0) {
            return 1;
        }

        unsigned char *next_block = (unsigned char *)(disk + block_ptr * EXT2_BLOCK_SIZE);
        memcpy(next_block, buf, amt_read);

        inode->i_size += amt_read;
        indirect_block[i] = block_ptr;
        inode->i_blocks += 1;
    }

    if (fread(buf, 1, EXT2_BLOCK_SIZE, src) > 0) {
        return 2;
    }
    else{
        return 0;
    }

}



