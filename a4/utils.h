#include "common.h"
#include "path_utils.h"
#include <sys/mman.h>
#include <errno.h>
#include "ext2.h"

void init_disk(char *img_name); 
PathData_t *split_path(char *path);
struct ext2_super_block *get_super_block();
struct ext2_group_desc *get_group_desc();
struct ext2_inode *get_inode_table();
unsigned char *get_inode_map();

int is_valid(unsigned char *inode_bitmap, int bit_idx);
int get_parent_inode(PathData_t *path_data); 
int new_dir_exists(int parent_inode, PathData_t *path_data);
int get_rec_len(char *dir_ent_name);
void init_dir_entry(int dir_block_num, int type, int inode_idx, char name[], int size);
int add_dir_to_parent(int parent_inode_num, int inode_idx, char name[]);