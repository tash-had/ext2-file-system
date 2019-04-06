#include "path_utils.h"
#include "utils.h"

#ifndef CSC369_EXT2_FS_H
#define CSC369_EXT2_FS_H
#endif

/*
* Return a dynamically allocated CharList with initialized values.
*/
PathNode_t *init_path_node(){
    PathNode_t *path_list = malloc(sizeof(PathNode_t));
    check_malloc(path_list);
    path_list->path_part = NULL;
    path_list->next = NULL;
    return path_list;
}


void add_path_part(PathNode_t *l, char *str){
    struct path_node *cur = l;
    while (cur->path_part != NULL) {
        cur = cur->next;
    }
    cur->next = init_path_node();
    cur->path_part = malloc((sizeof(char) * strlen(str)) + 1);
    check_malloc(cur->path_part);
    strcpy(cur->path_part, str);
}

/*
* Given a possibly null CharList and a possibly null IntList,
* free the memory allocated by each (if any).
*/
void free_path_list(PathNode_t *l){
    if (l != NULL && l->next != NULL){
        struct path_node *cur = l->next;
        while (cur != NULL) {
            struct path_node *last = cur;
            cur = cur->next;
            free(last->path_part);
            free(last);
        }
    }
    if (l != NULL) {
        free(l->path_part);
        free(l);
    }
}

PathData_t *split_path(char *path, char *file_name) {
    PathData_t *path_data = init_path_data();

    char *path_part = strtok(path, "/");
    while (path_part != NULL) {
        char *next_path_part = strtok(NULL, "/");
        if (next_path_part == NULL && !file_name) {
            // the path given is just a file name
            path_data->file_name = malloc((sizeof(char) * strlen(path_part)) + 1);
            strcpy(path_data->file_name, path_part);
        }
        else {
            add_path_part(path_data->path, path_part);
        }
        path_part = next_path_part;
    }
    if (file_name){
        path_data->file_name = malloc((sizeof(char) * strlen(file_name)) + 1);
        strcpy(path_data->file_name, file_name);
    }
    return path_data;
}

PathData_t *init_path_data() {
    PathData_t *pd = malloc(sizeof(PathData_t));
    pd->path = init_path_node();
    pd->file_name = NULL;
    return pd;
}

void free_path_data(PathData_t *pd) {
    free(pd->file_name);
    free_path_list(pd->path);
    free(pd);
}

int get_inode_with_path(PathData_t *pd) {
    struct ext2_inode *inode_table = get_inode_table();

    int parent_inode_num = get_parent_inode(pd);
    struct ext2_inode *parent_inode = &inode_table[parent_inode_num-1];
    struct ext2_dir_entry *curr_dir = (struct ext2_dir_entry *)(disk +
                                                                EXT2_BLOCK_SIZE*parent_inode->i_block[0]);

    int traversed_len = 0;
    int rec_len = 0;

    /**
     * TODO
     * FIX STRCMP EVERYWHERE -> CHANGE TO STRNCMP!!!
     */
    while (traversed_len < EXT2_BLOCK_SIZE){
        curr_dir = (void *)curr_dir + rec_len;
        if (strcmp(curr_dir->name, pd->file_name) == 0 && curr_dir->inode != 0){
            //dir already exists, abort mission
            return curr_dir->inode;
        }
        rec_len = curr_dir->rec_len;
        traversed_len += rec_len;
    }
    return -1;
}


/**
* malloc_res should a pointer returned by a malloc sys call.
*/
void check_malloc(void *malloc_res){
    if (malloc_res == NULL){
        fprintf(stderr, "A process was unable to allocate memory on the heap. Any output that follows is not to be trusted\n");
        exit(1);
    }
}