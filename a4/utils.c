#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "ext2.h"

#define BLOCK 0
#define INODE 1

int is_valid(unsigned char *inode_bitmap, int bit_idx){
    int mask = 1 << (bit_idx % 8);
    return inode_bitmap[bit_idx/8] & mask;
}

int allocate_next_free(unsigned char *disk, int type){
    struct ext2_super_block *sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
    struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE*2);
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

void init_inode(unsigned char *disk, int type, int inode_num, int block_num){
    struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE*2);
    struct ext2_inode *inode_table = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE*gd->bg_inode_table);
    struct ext2_inode new_inode;

    new_inode.i_mode = (unsigned short) type;
    new_inode.i_uid = 0;
    new_inode.i_size = EXT2_BLOCK_SIZE;
    new_inode.i_gid = 0;
    new_inode.i_links_count = 1;
    new_inode.i_blocks = 0;
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

void init_dir_entry(unsigned char *disk, int type, int inode_num, char name[]){}
