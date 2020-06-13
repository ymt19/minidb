#pragma once

#include "block.h"
#include "page.h"

typedef struct {
    int blksize;
} FileManager;

FileManager* new_FileManager(char*, unsigned int);
void fm_read(FileManager*, Block*, Page*);
void fm_write(FileManager*, Block*, Page*);
Block* fm_append_newblk(FileManager*, char *filename);
int fm_file_size(FileManager*, char *filename);
