#pragma once

#include "block.h"
#include "page.h"

/**
 * Blockは、データとchecksumで構成される。
 * <-----data_size-----><--checksum_size-->
 * <--------------blk_size---------------->
 * 
 * data_size    : データのみのサイズ
 * checksum_size: checksumのサイズ
 * blk_size     : data_size + checksum_size
 */
typedef struct {
    int data_size;          // データのみのサイズ
    int checksum_size;      // checksum用のサイズ
    int blk_size;           // Blockに含まれる全てのサイズ
} FileManager;

FileManager* new_FileManager(char*, unsigned int);
int fm_read(FileManager*, Block*, Page*);
void fm_write(FileManager*, Block*, Page*);
Block* fm_append_newblk(FileManager*, char*);
int fm_file_size(FileManager*, char*);
