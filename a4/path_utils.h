#include "common.h"

struct path_node {
    char *path_part;
    struct path_node *next;
};
typedef struct path_node PathNode_t;

void add_path_part(PathNode_t *pl, char *part);
void free_path_list(PathNode_t *pl);
void check_malloc(void *malloc_res);


struct path_data {
    char *file_name; // the new file to create
    PathNode_t *path; // up to parent of the file named file_name
};
typedef struct path_data PathData_t;

PathData_t *init_path_data();
void free_path_data(PathData_t *pd);

PathData_t *split_path(char *path, char *file_name);
int get_parent_inode(PathData_t *path_data);
int new_file_exists(int parent_inode, PathData_t *path_data, int type);
int get_inode_with_path(PathData_t *pd);
