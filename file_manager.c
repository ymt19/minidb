#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "file_manager.h"

FileManager* new_FileManager(char *pathname, int blksize) {
    FileManager* filemgr = malloc(sizeof(FileManager));
    if (filemgr == NULL) {
        return NULL;
    }

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

    filemgr->blksize = blksize;
    
    return filemgr;
}