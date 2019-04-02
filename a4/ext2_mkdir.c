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
    struct ext2_group_desc *gd = get_group_desc();
    struct ext2_inode *inode_table = get_inode_table();
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

    /**
     * TODO
     * - Free path_list (see free helper in path_utils.h)
     * - Free path_data struct (see free helper in path_utils.h)
     */


}