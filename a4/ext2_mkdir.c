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
#include "utils.c"

unsigned char *disk;


int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <image file name> <path>\n", argv[0]);
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);
        if(fd == -1) {
            perror("open");
            exit(1);
    }

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    char *path = argv[2];
    if (path[0] != '\\'){
        perror('Not absolute path');
        return ENOENT;
    }
    struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE*2);
    struct ext2_inode *inode_table = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE*gd->bg_inode_table);
    int free_block = allocate_next_free(disk, BLOCK);
    int free_inode = allocate_next_free(disk, INODE);
    if (free_block == -1 || free_inode == -1){
        perror("No space");
        return ENOSPC;
    }
    char *curr_token = strtok(path, "\\");
    char *next_token;
    while (curr_token != NULL){
        next_token = strtok(NULL, "\\");
        //find inode of token

        //if doesnt exist -> RETURN -EEXIST
        
        //if next_token is NULL
            //if curr_token already exist -> RETURN "path already exists"

            //else init + create dir_entry @ curr_token
        curr_token = next_token;
        

    }


}