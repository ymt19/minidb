#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "file_manager.h"

unsigned int g_blksize;

/**
 * Create new file manager.
 */
FileManager* new_FileManager(char *pathname, unsigned int blksize) {
    FileManager *fm;
    struct stat buf;
    if (stat(pathname, &buf) == -1) {
        // Directory does not exist.
        if (mkdir(pathname, 0777) == -1) {
            perror("mkdir");
            exit(1);
        }
    }
    if (chdir(pathname) == -1) {
        perror("chdir");
        exit(1);
    }

    fm = malloc(sizeof(FileManager));
    if (fm == NULL) {
        return NULL;
    }
    fm->blksize = blksize;
    return fm;
}

/**
 * Read the contents of the block in to the page. 
 */
void fm_read(FileManager *fm, Block *blk, Page *page) {
    // Read without buffering.
    int fd;
    if ((fd = open(blk->filename, O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }

    if (lseek(fd, blk->blk_number * fm->blksize, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }

    if (read(fd, page->data, fm->blksize) == -1) {
        perror("read");
        exit(1);
    }

    close(fd);
}

/**
 * Write the contents of the page to the block.
 */
void fm_write(FileManager *fm, Block *blk, Page *page) {
    // Write without buffering.
    int fd;
    if ((fd = open(blk->filename, O_WRONLY | O_CREAT, 0777)) == -1) {
        perror("open");
        exit(1);
    }

    if (lseek(fd, blk->blk_number * fm->blksize, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }

    if (write(fd, page->data, fm->blksize) == -1) {
        perror("read");
        exit(1);
    }

    close(fd);
}

/**
 * Append a block to the file.
 */
Block* fm_append_newblk(FileManager *fm, char *filename) {
    int new_blk_number = fm_file_size(fm, filename);
    Block *block = new_block(filename, new_blk_number);
    // empty byte array
    unsigned char *bytes = calloc(1, sizeof(unsigned char) * fm->blksize);

    int fd;
    if ((fd = open(filename, O_WRONLY | O_CREAT, 0777)) == -1) {
        perror("open");
        exit(1);
    }
    if (lseek(fd, new_blk_number * fm->blksize, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }
    if (write(fd, bytes, fm->blksize) == -1) {
        perror("read");
        exit(1);
    }
    close(fd);

    free(bytes);
    return block;
}

/**
 * Get the size of the file in a unit of blocks.
 * @return size on success -1 on the file is not exist.
 */
int fm_file_size(FileManager *fm, char *filename) {
    struct stat buf;
    if (stat(filename, &buf) == -1) {
        // file doesnt exist or file access erorr
        return 0;
    }
    return (int)(buf.st_size/fm->blksize);
}