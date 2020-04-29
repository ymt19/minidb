#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "file_manager.h"

int g_blksize;

void new_FileManager(char *pathname, int blksize) {
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