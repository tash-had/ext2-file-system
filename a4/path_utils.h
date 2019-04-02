#include "common.h"

struct path_node {
    char *path_part;
    struct path_node *next;
};
typedef struct path_node PathNode_t;



PathNode_t *init_path_list();
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


