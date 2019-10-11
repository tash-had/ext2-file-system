# ext2 CLI Tools 
## [Click here for handout](https://docs.google.com/document/d/1hIa0XTumToHp72mY1HtWiSOvrA0eAcSOoE5FFMduJww/edit?usp=sharing)

Implementation of command line tools for the EXT2 File System. This was done for CSC369 - Operating Systems Assignment 4. Each tool takes a disk image which is setup in a particular way to test our implementation. All images that can be passed in are [here](https://github.com/tash-had/ext2-file-system/tree/master/images). We implement the following: 

- `mkdir <img> <name>`: Make a new directory called "name"
- `cp <img> <src> <dest>` : Copy `src` to `dest`
- `ln <img> <src> <dst>`: Create a link to `src` in `dest`
- `rm <img> <src>` : Remove `src`
- `restore <img> <src>` : Restore the removed file `src` 
- `checker <img>` : Check the disk image `img` 
