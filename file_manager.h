#pragma once

#include "block.h"
#include "page.h"

typedef struct {
    int data_size;          // データのみのサイズ
    int checksum_size;      // checksum用のサイズ
    int blk_size;           // Blockに含まれる全てのサイズ
} FileManager;

FileManager* new_FileManager(char*, unsigned int);
int fm_read(FileManager*, Block*, Page*);
int fm_write(FileManager*, Block*, Page*);
Block* fm_append_newblk(FileManager*, char*);
int fm_file_size(FileManager*, char*);
