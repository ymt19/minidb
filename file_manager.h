#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "block.h"
#include "page.h"

extern unsigned int g_blksize;

void new_FileManager(char*, unsigned int);
void read_page_from_blk(Block*, Page*);
void write_page_to_blk(Block*, Page*);
Block* append_newblk_fm(char filename[MAX_FILENAME]);
int file_size(char filename[MAX_FILENAME]);

#endif