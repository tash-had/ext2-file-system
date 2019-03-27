#include <stdio.h>
#include "string.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "ext2.h"

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
}