#include "common.h"

#define BLOCK 0
#define INODE 1

unsigned char *disk;




struct ext2_super_block *get_super_block();
struct ext2_group_desc *get_group_desc();
struct ext2_inode *get_inode_table();
unsigned char *get_block_bitmap();
unsigned char *get_inode_map();

void init_disk(char *img_name);
int is_valid(unsigned char *bitmap, int bit_idx);
int get_rec_len(char *dir_ent_name);
void init_dir_entry(int dir_block_num, int offset,  int type, int inode_idx, char name[], int size);
int add_file_to_parent(int parent_inode_num, int inode_num, char name[], int type);
int allocate_next_free(int type);
void init_inode(int type, int inode_num, int block_num);
void check_valid(void *ext2_struct, char *item);
struct ext2_inode *get_inode_with_num(unsigned int inode_number);
int copy_to_fs(FILE *src, struct ext2_inode *inode, int block_num);
void deallocate(int index, int type);
int get_first_unused_block_idx(int inode_num);
int fix_inodes_count();
int fix_blocks_count();
unsigned char get_type(int mode);
int fix_mode_type();
int fix_inode_allocation();
int fix_dtime();
int fix_block_allocation();
int total_inconsistencies();
void allocate_block_with_num(unsigned int block_num);
void allocate_inode_with_num(unsigned int inode_num);