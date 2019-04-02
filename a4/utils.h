#include "common.h"
#include "path_utils.h"
#include <sys/mman.h>
#include <errno.h>
#include "ext2.h"

PathData_t *split_path(char *path);
struct ext2_super_block *get_super_block();
struct ext2_group_desc *get_group_desc();
struct ext2_inode *get_inode_table();
