#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "file_manager.h"

unsigned int g_blksize;

void new_FileManager(char *pathname, unsigned int blksize) {
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

    g_blksize = blksize;
}

void read_page_from_blk(Block *blk, Page *page) {
    // Read without buffering.
    int fd;
    if ((fd = open(blk->filename, O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }

    if (lseek(fd, blk->blk_number * g_blksize, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }

    if (read(fd, page->data, g_blksize) == -1) {
        perror("read");
        exit(1);
    }

    close(fd);
}

void write_page_to_blk(Block *blk, Page *page) {
    // Write without buffering.
    int fd;
    if ((fd = open(blk->filename, O_WRONLY | O_CREAT, 0777)) == -1) {
        perror("open");
        exit(1);
    }

    if (lseek(fd, blk->blk_number * g_blksize, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }

    if (write(fd, page->data, g_blksize) == -1) {
        perror("read");
        exit(1);
    }

    close(fd);
}

Block* append_newblk_fm(char filename[MAX_FILENAME]) {
    int new_blk_number = file_size(filename);
    Block *block = new_block(filename, new_blk_number);
    // empty byte array
    unsigned char* bytes = malloc(sizeof(unsigned char) * g_blksize);

    int fd;
    if ((fd = open(filename, O_WRONLY | O_CREAT, 0777)) == -1) {
        perror("open");
        exit(1);
    }
    if (lseek(fd, new_blk_number * g_blksize, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }
    if (write(fd, bytes, g_blksize) == -1) {
        perror("read");
        exit(1);
    }
    close(fd);

    free(bytes);

    return block;
}

// The number of blocks in the file.
int file_size(char filename[MAX_FILENAME]) {
    struct stat buf;
    if (stat(filename, &buf) == -1) {
        return -1;
    }
    return (int)(buf.st_size/g_blksize);
}