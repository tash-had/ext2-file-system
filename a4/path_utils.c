#include "path_utils.h"



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


/*
* Given an char, c, and a CharList, l, add c to l.
*/
void add_path_part(PathNode_t *l, char *str){
    struct path_node *cur = l;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = init_path_node();
    cur->next->path_part = malloc((sizeof(char) * strlen(str)) + 1);
    check_malloc(cur->next->path_part);
    strcpy(cur->next->path_part, str);
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

/**
* malloc_res should a pointer returned by a malloc sys call.
*/
void check_malloc(void *malloc_res){
    if (malloc_res == NULL){
        fprintf(stderr, "A process was unable to allocate memory on the heap. Any output that follows is not to be trusted\n");
        exit(1);
    }
}